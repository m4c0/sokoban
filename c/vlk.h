#ifndef VLK_H
#define VLK_H

#ifdef __APPLE__
CAMetalLayer * vlk_metal_layer();
#endif

#ifdef _WIN32
extern HWND vlk_hwnd;
#endif

static VkCommandPool      vlk_cpool;
static VkDevice           vlk_dev;
static VkExtent2D         vlk_ext;
static VkInstance         vlk_ins;
static VkPhysicalDevice   vlk_pd;
static VkQueue            vlk_q;
static VkSurfaceFormatKHR vlk_surf_fmt;
static VkSurfaceKHR       vlk_surf;
static unsigned           vlk_qf;
static unsigned           vlk_swc_count;

#define MAX_SWAPCHAIN_IMAGES 8
typedef struct vlk_swc {
  VkFramebuffer   fb  [MAX_SWAPCHAIN_IMAGES];
  VkImageView     iv  [MAX_SWAPCHAIN_IMAGES];
  VkSwapchainKHR  swc;
} vlk_swc_t;

static vlk_swc_t vlk_swc     = {0};
static vlk_swc_t vlk_swc_old = {0};

static VkCommandBuffer vlk_cb      [MAX_SWAPCHAIN_IMAGES];
static VkImage         vlk_swc_img [MAX_SWAPCHAIN_IMAGES];

#define MAX_INFLIGHTS 3
static VkFence     vlk_fence        [MAX_INFLIGHTS];
static VkSemaphore vlk_sema_img     [MAX_INFLIGHTS];
static VkSemaphore vlk_sema_present [MAX_INFLIGHTS];
static unsigned    vlk_cur_inflight;

void vlk_log(int r, const char * msg);
static void vlk_check(VkResult r, const char * msg) {
  if (r == VK_SUCCESS) return;
  vlk_log(r, msg);
  exit(1);
}
#define _(X) vlk_check((X), #X)

static void vlk_create_instance() {
  const char * ext[] = {
    0, // Platform-specific
    VK_KHR_SURFACE_EXTENSION_NAME,
    // Next two are only used by OSX
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
  };

  VkApplicationInfo app = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion = VK_API_VERSION_1_0,
  };
  VkInstanceCreateInfo info = (VkInstanceCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &app,
    .ppEnabledExtensionNames = ext,
    .enabledExtensionCount = 2,
  };

#ifdef __APPLE__
  ext[0] = VK_EXT_METAL_SURFACE_EXTENSION_NAME;

#if !TARGET_OS_IPHONE
  // MoltenVK kinda requires this extension/flag. It works without it, but the
  // validation layer will complain.
  info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  info.enabledExtensionCount += 2;
#endif
#elif _WIN32
  ext[0] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
#  error Unsupported platform
#endif

  _(vkCreateInstance(&info, NULL, &vlk_ins));

#if !TARGET_OS_IPHONE
  volkLoadInstance(vlk_ins);
#endif
}

static void vlk_find_physical_device() {
  VkPhysicalDevice pd[16];
  uint32_t pdsz = 16;
  _(vkEnumeratePhysicalDevices(vlk_ins, &pdsz, pd));
  for (int i = 0; i < pdsz; i++) {
    VkQueueFamilyProperties qp[16];
    uint32_t qpsz = 16;
    vkGetPhysicalDeviceQueueFamilyProperties(pd[i], &qpsz, qp);
    for (vlk_qf = 0; vlk_qf < qpsz; vlk_qf++) {
      if ((qp[vlk_qf].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0) continue;
      if ((qp[vlk_qf].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0) continue;
      vlk_pd = pd[i];
      return;
    }
  }
  assert(0);
}

static void vlk_create_device() {
  const char * ext[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    "VK_KHR_portability_subset",
  };

  VkPhysicalDeviceSynchronization2FeaturesKHR sync2 = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
    .synchronization2 = VK_TRUE,
  };

  const float pri = 1.0f;
  VkDeviceQueueCreateInfo q = (VkDeviceQueueCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueCount = 1,
    .pQueuePriorities = &pri,
    .queueFamilyIndex = vlk_qf,
  };
  VkDeviceCreateInfo info = (VkDeviceCreateInfo) {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &sync2,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &q,
    .ppEnabledExtensionNames = ext,
    .enabledExtensionCount = 2,
  };

#ifdef __APPLE__
  // It would be more "Vulkan-idiomatic" to test if the current instance has
  // the portability flag.
  info.ppEnabledExtensionNames = ext;
  info.enabledExtensionCount++;
#endif

  _(vkCreateDevice(vlk_pd, &info, NULL, &vlk_dev));
#if !TARGET_OS_IPHONE
  volkLoadDevice(vlk_dev);
#endif

  vkGetDeviceQueue(vlk_dev, vlk_qf, 0, &vlk_q);
}

static void vlk_create_surface() {
#ifdef __APPLE__
  VkMetalSurfaceCreateInfoEXT info = {
    .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
    .pLayer = vlk_metal_layer(),
  };
  _(vkCreateMetalSurfaceEXT(vlk_ins, &info, NULL, &vlk_surf));
#elif _WIN32
  assert(vlk_hwnd);
  VkWin32SurfaceCreateInfoKHR info = {
    .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .hinstance = GetModuleHandle(NULL),
    .hwnd      = vlk_hwnd,
  };
  _(vkCreateWin32SurfaceKHR(vlk_ins, &info, NULL, &vlk_surf));
#else
#error Unsupported platform
#endif

  VkSurfaceCapabilitiesKHR cap;
  _(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vlk_pd, vlk_surf, &cap));

  vlk_swc_count = cap.minImageCount + 1;
  if (vlk_swc_count > cap.maxImageCount && cap.maxImageCount > 0) vlk_swc_count = cap.maxImageCount;
  assert(vlk_swc_count < MAX_SWAPCHAIN_IMAGES);

  // No concensus on docs or the Internet about how to deal with surface
  // formats. Picking the first seems to be enough for most cases.
  uint32_t sz = 1;
  VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(vlk_pd, vlk_surf, &sz, &vlk_surf_fmt);
  if (res != VK_INCOMPLETE) vlk_check(res, "vkGetPhysicalDeviceSurfaceFormatsKHR invalid return");
}

static void vlk_create_swapchain() {
  VkSwapchainCreateInfoKHR info = {
    .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface          = vlk_surf,
    .minImageCount    = vlk_swc_count,
    .imageFormat      = vlk_surf_fmt.format,
    .imageColorSpace  = vlk_surf_fmt.colorSpace,
    .imageExtent      = vlk_ext,
    .imageArrayLayers = 1,
    // In theory we can add more usages as well
    .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    // We can use others modes, if supported, for max FPS or discard frames if
    // CPU faster than GPU
    .presentMode      = VK_PRESENT_MODE_FIFO_KHR,
    // Should be "true" unless we want to read clipped parts
    .clipped          = VK_TRUE,
    .oldSwapchain     = vlk_swc_old.swc,
  };
  _(vkCreateSwapchainKHR(vlk_dev, &info, NULL, &vlk_swc.swc));

  _(vkGetSwapchainImagesKHR(vlk_dev, vlk_swc.swc, &vlk_swc_count, vlk_swc_img));

  for (int i = 0; i < vlk_swc_count; i++) {
    VkImageViewCreateInfo info = {
      .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image            = vlk_swc_img[i],
      .viewType         = VK_IMAGE_VIEW_TYPE_2D,
      .format           = vlk_surf_fmt.format,
      .subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount     = 1,
        .layerCount     = 1,
      },
    };
    _(vkCreateImageView(vlk_dev, &info, NULL, vlk_swc.iv + i));
  }
}

static void vlk_create_framebuffer();
static void vlk_destroy_swc(vlk_swc_t * swc) {
  for (int i = 0; i < vlk_swc_count; i++) {
    vkDestroyFramebuffer(vlk_dev, swc->fb[i], NULL);
    vkDestroyImageView(vlk_dev, swc->iv[i], NULL);
  }
  vkDestroySwapchainKHR(vlk_dev, swc->swc, NULL);

  *swc = (vlk_swc_t) {0};
}
static void vlk_create_swc() {
  if (vlk_swc_old.swc) vlk_destroy_swc(&vlk_swc_old);
  vlk_swc_old = vlk_swc;

  VkSurfaceCapabilitiesKHR cap;
  _(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vlk_pd, vlk_surf, &cap));
  vlk_ext = cap.currentExtent;

  vlk_create_swapchain();
  vlk_create_framebuffer();
}

FILE * vlk_open(const char * name, const char * ext);
static VkShaderModule vlk_create_shader_module(const char * name) {
  FILE * f = vlk_open(name, "spv");
  assert(f);
  assert(0 == fseek(f, 0, SEEK_END));
  long sz = ftell(f);
  assert(sz && (sz % 4 == 0));
  assert(0 == fseek(f, 0, SEEK_SET));
  uint32_t * data = malloc(sz);
  assert(1 == fread(data, sz, 1, f));
  fclose(f);

  VkShaderModuleCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = sz,
    .pCode = data,
  };

  VkShaderModule mod;
  _(vkCreateShaderModule(vlk_dev, &info, NULL, &mod));

  free(data);
  return mod;
}

static VkBuffer vlk_create_buffer_for_image(unsigned sz) {
  VkBufferCreateInfo buf_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size  = sz,
    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  };
  VkBuffer buf;
  _(vkCreateBuffer(vlk_dev, &buf_info, NULL, &buf));
  return buf;
}

static int vlk_find_memory(VkMemoryPropertyFlags desired) {
  VkPhysicalDeviceMemoryProperties props;
  vkGetPhysicalDeviceMemoryProperties(vlk_pd, &props);

  for (int i = 0; i < props.memoryTypeCount; i++) {
    VkMemoryPropertyFlags flags = props.memoryTypes[i].propertyFlags;
    if ((flags & desired) == desired) return i;
  }
  assert(0 && "could not find suitable vulkan memory");
  return -1; // unreachable
}
static int vlk_find_host_memory() {
  return vlk_find_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
static int vlk_find_local_memory() {
  return vlk_find_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

static VkDeviceMemory vlk_allocate_memory(VkDeviceSize sz, int idx) {
  VkMemoryAllocateInfo mem_info = {
    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .allocationSize  = sz,
    .memoryTypeIndex = idx,
  };
  VkDeviceMemory mem;
  _(vkAllocateMemory(vlk_dev, &mem_info, NULL, &mem));
  return mem;
}

static VkImage vlk_create_image(unsigned w, unsigned h, VkFormat fmt, VkImageUsageFlagBits flags) {
  VkImageCreateInfo img_info = {
    .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType   = VK_IMAGE_TYPE_2D,
    .extent      = (VkExtent3D) { w, h, 1 },
    .usage       = VK_IMAGE_USAGE_SAMPLED_BIT | flags,
    .samples     = VK_SAMPLE_COUNT_1_BIT,
    .format      = fmt,
    .mipLevels   = 1,
    .arrayLayers = 1,
  };
  VkImage img;
  _(vkCreateImage(vlk_dev, &img_info, NULL, &img));
  return img;
}
static VkDeviceMemory vlk_allocate_image_memory(VkImage img) {
  VkMemoryRequirements req;
  vkGetImageMemoryRequirements(vlk_dev, img, &req);

  VkDeviceMemory mem = vlk_allocate_memory(req.size, vlk_find_local_memory());
  _(vkBindImageMemory(vlk_dev, img, mem, 0));
  return mem;
}
static VkImageView vlk_create_image_view(VkImage img, VkFormat fmt) {
  VkImageViewCreateInfo iv_info = {
    .sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image        = img,
    .format       = fmt,
    .viewType     = VK_IMAGE_VIEW_TYPE_2D,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .levelCount = 1,
      .layerCount = 1,
    },
  };
  VkImageView iv;
  _(vkCreateImageView(vlk_dev, &iv_info, NULL, &iv));
  return iv;
}

static void vlk_create_command_pool() {
  VkCommandPoolCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
  };
  _(vkCreateCommandPool(vlk_dev, &info, NULL, &vlk_cpool));
}

static void vlk_allocate_command_buffers(int count, VkCommandBuffer * cbs) {
  VkCommandBufferAllocateInfo info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = vlk_cpool,
    .commandBufferCount = count,
  };
  _(vkAllocateCommandBuffers(vlk_dev, &info, cbs));
}

static void vlk_create_semaphores() {
  VkSemaphoreCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  for (int i = 0; i < MAX_INFLIGHTS; i++) {
    _(vkCreateSemaphore(vlk_dev, &info, NULL, vlk_sema_img     + i));
    _(vkCreateSemaphore(vlk_dev, &info, NULL, vlk_sema_present + i));
  }
}

static void vlk_create_fences() {
  VkFenceCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  for (int i = 0; i < MAX_INFLIGHTS; i++) {
    _(vkCreateFence(vlk_dev, &info, NULL, vlk_fence + i));
  }
}

static void vlk_create() {
#if !TARGET_OS_IPHONE
  _(volkInitialize());
#endif

  vlk_create_instance();
  vlk_find_physical_device();
  vlk_create_surface();
  vlk_create_device();

  vlk_create_semaphores();
  vlk_create_fences();

  vlk_create_command_pool();
  vlk_allocate_command_buffers(vlk_swc_count, vlk_cb);
}

static void vlk_destroy() {
  vlk_destroy_swc(&vlk_swc);
  vlk_destroy_swc(&vlk_swc_old);

  for (int i = 0; i < MAX_INFLIGHTS; i++) {
    vkDestroyFence    (vlk_dev, vlk_fence       [i], NULL);
    vkDestroySemaphore(vlk_dev, vlk_sema_img    [i], NULL);
    vkDestroySemaphore(vlk_dev, vlk_sema_present[i], NULL);
  }

  vkDestroyCommandPool(vlk_dev, vlk_cpool, NULL);

  vkDestroyDevice(vlk_dev, NULL);
  vkDestroySurfaceKHR(vlk_ins, vlk_surf, NULL);
  vkDestroyInstance(vlk_ins, NULL);
}

#endif
