#ifndef VLK_GAME_H
#define VLK_GAME_H

void vlk_init();
void vlk_frame();
void vlk_deinit();

void vlk_cursor(int dx, int dy);
void vlk_toggle();

void vlk_load();
void vlk_save();

#ifdef VLK_IMPL
#include "vlk.h"

typedef struct vlk_upc {
  int x, y;
} vlk_upc_t;

static vlk_upc_t vlk_pc;

static VkCommandPool      vlk_cpool;
static VkRenderPass       vlk_rp;

static VkBuffer       vlk_atlas_h_buf;
static VkDeviceMemory vlk_atlas_h_mem;

static VkDeviceMemory vlk_atlas_mem;
static VkImage        vlk_atlas_img;
static VkImageView    vlk_atlas_iv;

static VkDescriptorPool      vlk_dpool;
static VkDescriptorSetLayout vlk_dsl;
static VkDescriptorSet       vlk_dset;
static VkPipelineLayout      vlk_pl;
static VkPipeline            vlk_ppl;
static VkSampler             vlk_smp;

static void vlk_create_render_pass() {
  VkAttachmentDescription att = {
    .format      = vlk_surf_fmt.format,
    .samples     = VK_SAMPLE_COUNT_1_BIT,
    .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference ref = {
    .attachment = 0,
    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  // Without this, we might face WRITE_AFTER_READ access hazards
  VkSubpassDependency dep = {
    .srcSubpass    = VK_SUBPASS_EXTERNAL,
    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
  };

  VkSubpassDescription subpass = {
    .colorAttachmentCount = 1,
    .pColorAttachments    = &ref,
  };

  VkRenderPassCreateInfo info = {
    .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .attachmentCount = 1,
    .pAttachments    = &att,
    .subpassCount    = 1,
    .pSubpasses      = &subpass,
    .dependencyCount = 1,
    .pDependencies   = &dep,
  };
  _(vkCreateRenderPass(vlk_dev, &info, NULL, &vlk_rp));
}

static void vlk_create_framebuffer() {
  for (int i = 0; i < vlk_swc_count; i++) {
    VkFramebufferCreateInfo info = {
      .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass      = vlk_rp,
      .attachmentCount = 1,
      .pAttachments    = vlk_swc.iv + i,
      .width           = vlk_ext.width,
      .height          = vlk_ext.height,
      .layers          = 1,
    };
    _(vkCreateFramebuffer(vlk_dev, &info, NULL, vlk_swc.fb + i));
  }
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

static VkCommandBuffer vlk_record_buf2img(VkBuffer buf, VkImage img, int w, int h) {
  VkCommandBuffer cb;
  vlk_allocate_command_buffers(1, &cb);

  VkCommandBufferBeginInfo binfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
  };
  vkBeginCommandBuffer(cb, &binfo);

  VkDependencyInfoKHR di = {
    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
    .bufferMemoryBarrierCount = 1,
    .pBufferMemoryBarriers    = (VkBufferMemoryBarrier2KHR[]) {{
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR,
      .srcStageMask  = VK_PIPELINE_STAGE_HOST_BIT,
      .dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT,
      .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .buffer        = buf,
      .size          = VK_WHOLE_SIZE,
    }},
    .imageMemoryBarrierCount = 1,
    .pImageMemoryBarriers    = (VkImageMemoryBarrier2KHR[]) {{
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
      .srcStageMask     = VK_PIPELINE_STAGE_HOST_BIT,
      .dstStageMask     = VK_PIPELINE_STAGE_TRANSFER_BIT,
      .dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT,
      .newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .image            = img,
      .subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount     = 1,
        .layerCount     = 1,
      },
    }},
  };
  vkCmdPipelineBarrier2KHR(cb, &di);

  VkBufferImageCopy bic = {
    .imageExtent = (VkExtent3D) { w, h, 1 },
    .imageSubresource = {
      .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
      .layerCount     = 1,
    },
  };
  vkCmdCopyBufferToImage(cb, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bic);

  di = (VkDependencyInfoKHR) {
    .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
    .imageMemoryBarrierCount = 1,
    .pImageMemoryBarriers    = (VkImageMemoryBarrier2KHR[]) {{
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
      .srcStageMask     = VK_PIPELINE_STAGE_TRANSFER_BIT,
      .dstStageMask     = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      .srcAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask    = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .image            = img,
      .subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount     = 1,
        .layerCount     = 1,
      },
    }},
  };
  vkCmdPipelineBarrier2KHR(cb, &di);

  vkEndCommandBuffer(cb);

  VkSubmitInfo submit = {
    .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pCommandBuffers    = &cb,
    .commandBufferCount = 1,
  };
  _(vkQueueSubmit(vlk_q, 1, &submit, NULL));

  return cb;
}

static void vlk_record_cmdbuf(int i) {
  VkCommandBuffer cb = vlk_cb[i];

  VkCommandBufferBeginInfo binfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };
  vkBeginCommandBuffer(cb, &binfo);

  VkClearValue clear = {
    .color = {{ 0.1, 0.2, 0.3, 1 }},
  };
  VkRenderPassBeginInfo rp = {
    .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass      = vlk_rp,
    .framebuffer     = vlk_swc.fb[i],
    .renderArea      = (VkRect2D) { .extent = vlk_ext },
    .clearValueCount = 1,
    .pClearValues    = &clear,
  };
  vkCmdBeginRenderPass(cb, &rp, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport vp = {
    .width  = vlk_ext.width,
    .height = vlk_ext.height,
  };
  vkCmdSetViewport(cb, 0, 1, &vp);

  VkRect2D sci = {
    .extent = vlk_ext,
  };
  vkCmdSetScissor(cb, 0, 1, &sci);

  vkCmdPushConstants(cb, vlk_pl, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vlk_upc_t), &vlk_pc);
  vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_ppl);
  vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_pl, 0, 1, &vlk_dset, 0, NULL);
  vkCmdDraw(cb, 3, 1, 0, 0);

  vkCmdEndRenderPass(cb);
  vkEndCommandBuffer(cb);
}

static void vlk_load_atlas() {
  FILE * f = fopen("atlas.img", "rb");;
  assert(f);
  assert(0 == fseek(f, 0, SEEK_END));
  long sz = ftell(f);
  assert(sz);
  assert(0 == fseek(f, 0, SEEK_SET));

  vlk_atlas_h_buf = vlk_create_buffer_for_image(sz);
  vlk_atlas_h_mem = vlk_allocate_memory(sz, vlk_find_host_memory());
  _(vkBindBufferMemory(vlk_dev, vlk_atlas_h_buf, vlk_atlas_h_mem, 0));

  void * data;
  _(vkMapMemory(vlk_dev, vlk_atlas_h_mem, 0, VK_WHOLE_SIZE, 0, &data));
  assert(1 == fread(data, sz, 1, f));
  vkUnmapMemory(vlk_dev, vlk_atlas_h_mem);
  fclose(f);

  vlk_atlas_img = vlk_create_image(128, 32, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  vlk_atlas_mem = vlk_allocate_image_memory(vlk_atlas_img);
  vlk_atlas_iv  = vlk_create_image_view(vlk_atlas_img, VK_FORMAT_R8_UNORM);

  vlk_record_buf2img(vlk_atlas_h_buf, vlk_atlas_img, 128, 32);
}

void vlk_init() {
#if !TARGET_OS_IPHONE
  _(volkInitialize());
#endif

  vlk_create_instance();
  vlk_find_physical_device();
  vlk_create_surface();
  vlk_create_device();
  vlk_create_command_pool();
  vlk_create_render_pass();
  vlk_create_swc();
  vlk_create_semaphores();
  vlk_create_fences();

  vlk_allocate_command_buffers(vlk_swc_count, vlk_cb);

  vlk_create_swc();

  vlk_load_atlas();

  VkDescriptorSetLayoutCreateInfo dsl_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .bindingCount = 1,
    .pBindings = (VkDescriptorSetLayoutBinding[]) {{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    }},
  };
  _(vkCreateDescriptorSetLayout(vlk_dev, &dsl_info, NULL, &vlk_dsl));

  VkDescriptorPoolCreateInfo dpool_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .maxSets = 1,
    .poolSizeCount = 1,
    .pPoolSizes = (VkDescriptorPoolSize[]) {{
      .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
    }},
  };
  _(vkCreateDescriptorPool(vlk_dev, &dpool_info, NULL, &vlk_dpool));

  VkDescriptorSetAllocateInfo dset_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .descriptorPool = vlk_dpool,
    .descriptorSetCount = 1,
    .pSetLayouts = &vlk_dsl,
  };
  _(vkAllocateDescriptorSets(vlk_dev, &dset_info, &vlk_dset));

  VkSamplerCreateInfo smp_info = {
    .sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,
  };
  _(vkCreateSampler(vlk_dev, &smp_info, NULL, &vlk_smp));

  VkWriteDescriptorSet wds = {
    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .dstSet          = vlk_dset,
    .dstBinding      = 0,
    .descriptorCount = 1,
    .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo      = (VkDescriptorImageInfo[]) {{
      .sampler       = vlk_smp,
      .imageView     = vlk_atlas_iv,
      .imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    }},
  };
  vkUpdateDescriptorSets(vlk_dev, 1, &wds, 0, NULL);

  VkPipelineLayoutCreateInfo pl_info = {
    .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount         = 1,
    .pSetLayouts            = &vlk_dsl,
    .pushConstantRangeCount = 1,
    .pPushConstantRanges    = (VkPushConstantRange[]) {{
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .size       = sizeof(vlk_upc_t),
    }},
  };
  _(vkCreatePipelineLayout(vlk_dev, &pl_info, NULL, &vlk_pl));

  VkShaderModule vert = vlk_create_shader_module("bited.vert");
  VkShaderModule frag = vlk_create_shader_module("bited.frag");

  VkGraphicsPipelineCreateInfo ppl_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .stageCount = 2,
    .pStages  = (VkPipelineShaderStageCreateInfo[]) {{
      .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage  = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vert,
      .pName  = "main",
    }, {
      .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = frag,
      .pName  = "main",
    }},
    .pVertexInputState = (VkPipelineVertexInputStateCreateInfo[]) {{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    }},
    .pInputAssemblyState = (VkPipelineInputAssemblyStateCreateInfo[]) {{
      .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    }},
    .pViewportState  = (VkPipelineViewportStateCreateInfo[]) {{
      .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount  = 1,
    }},
    .pRasterizationState = (VkPipelineRasterizationStateCreateInfo[]) {{
      .sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth   = 1,
    }},
    .pMultisampleState = (VkPipelineMultisampleStateCreateInfo[]) {{
      .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    }},
    .pColorBlendState = (VkPipelineColorBlendStateCreateInfo[]) {{
      .sType            = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOp          = VK_LOGIC_OP_COPY,
      .attachmentCount  = 1,
      .pAttachments     = (VkPipelineColorBlendAttachmentState[]) {{
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
      }},
    }},
    .pDynamicState = (VkPipelineDynamicStateCreateInfo[]) {{
      .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 2,
      .pDynamicStates    = (VkDynamicState[]) {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
      },
    }},
    .layout     = vlk_pl,
    .renderPass = vlk_rp,
  };
  _(vkCreateGraphicsPipelines(vlk_dev, NULL, 1, &ppl_info, NULL, &vlk_ppl));

  vkDestroyShaderModule(vlk_dev, vert, NULL);
  vkDestroyShaderModule(vlk_dev, frag, NULL);
}

void vlk_frame() {
  unsigned inf = vlk_cur_inflight;

  _(vkWaitForFences(vlk_dev, 1, vlk_fence + inf, VK_TRUE, ~0UL));
  _(vkResetFences  (vlk_dev, 1, vlk_fence + inf));

  unsigned idx;
  vkAcquireNextImageKHR(vlk_dev, vlk_swc.swc, ~0UL, vlk_sema_img[inf], VK_NULL_HANDLE, &idx);

  vlk_record_cmdbuf(idx);

  // TODO: confirm if this is the best and document why
  VkPipelineStageFlags stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  // The idea of the wait semaphore is to wait until the swapchain _actually_
  // made the image available
  VkSubmitInfo submit = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pCommandBuffers = vlk_cb + idx,
    .commandBufferCount = 1,
    .pWaitSemaphores = vlk_sema_img + inf,
    .pWaitDstStageMask = &stage,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = vlk_sema_present + inf,
    .signalSemaphoreCount = 1,
  };
  // The fence signals we can reuse the current in-flight
  _(vkQueueSubmit(vlk_q, 1, &submit, vlk_fence[inf]));

  // Present is entirely async. We don't have control when it finishes. We then
  // use a semaphore to force it to wait until we finished processing the
  // image.
  VkPresentInfoKHR pres = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pWaitSemaphores = vlk_sema_present + inf,
    .waitSemaphoreCount = 1,
    .swapchainCount = 1,
    .pSwapchains = &vlk_swc.swc,
    .pImageIndices = &idx,
  };
  VkResult res = vkQueuePresentKHR(vlk_q, &pres);
  // TODO: deal with suboptimal
  if (res != VK_SUBOPTIMAL_KHR) {
    vlk_check(res, "vkQueuePresentKHR");
  } else {
    VkSurfaceCapabilitiesKHR cap;
    _(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vlk_pd, vlk_surf, &cap));
    vlk_ext = cap.currentExtent;

    vlk_create_swc();
  }
}

void vlk_deinit() {
  vkDeviceWaitIdle(vlk_dev);

  vlk_destroy_swc(&vlk_swc);
  vlk_destroy_swc(&vlk_swc_old);

  for (int i = 0; i < MAX_INFLIGHTS; i++) {
    vkDestroyFence    (vlk_dev, vlk_fence       [i], NULL);
    vkDestroySemaphore(vlk_dev, vlk_sema_img    [i], NULL);
    vkDestroySemaphore(vlk_dev, vlk_sema_present[i], NULL);
  }

  vkDestroyBuffer(vlk_dev, vlk_atlas_h_buf, NULL);
  vkFreeMemory   (vlk_dev, vlk_atlas_h_mem, NULL);

  vkDestroyImageView (vlk_dev, vlk_atlas_iv, NULL);
  vkDestroyImage     (vlk_dev, vlk_atlas_img, NULL);
  vkFreeMemory       (vlk_dev, vlk_atlas_mem, NULL);

  vkDestroySampler             (vlk_dev, vlk_smp,   NULL);
  vkDestroyDescriptorSetLayout (vlk_dev, vlk_dsl,   NULL);
  vkDestroyDescriptorPool      (vlk_dev, vlk_dpool, NULL);
  vkDestroyPipeline            (vlk_dev, vlk_ppl,   NULL);
  vkDestroyPipelineLayout      (vlk_dev, vlk_pl,    NULL);

  vkDestroyCommandPool(vlk_dev, vlk_cpool, NULL);
  vkDestroyRenderPass(vlk_dev, vlk_rp, NULL);
  vkDestroyDevice(vlk_dev, NULL);
  vkDestroySurfaceKHR(vlk_ins, vlk_surf, NULL);
  vkDestroyInstance(vlk_ins, NULL);
}

void vlk_cursor(int dx, int dy) {
  int x = vlk_pc.x + dx;
  if (x >= 0 && x < 128) vlk_pc.x = x;

  int y = vlk_pc.y + dy;
  if (y >= 0 && y < 128) vlk_pc.y = y;
}

void vlk_toggle() {
  unsigned char * data;
  _(vkMapMemory(vlk_dev, vlk_atlas_h_mem, 0, VK_WHOLE_SIZE, 0, (void **)&data));

  int i = vlk_pc.y * 128 + vlk_pc.x;
  data[i] = data[i] ? 0 : 255;

  vkUnmapMemory(vlk_dev, vlk_atlas_h_mem);
  vlk_record_buf2img(vlk_atlas_h_buf, vlk_atlas_img, 128, 32);
}

void vlk_load() {
  FILE * f = fopen("atlas.img", "rb");

  unsigned char * data;
  _(vkMapMemory(vlk_dev, vlk_atlas_h_mem, 0, VK_WHOLE_SIZE, 0, (void **)&data));
  fread(data, 128 * 32, 1, f);
  vkUnmapMemory(vlk_dev, vlk_atlas_h_mem);

  fclose(f);
  vlk_record_buf2img(vlk_atlas_h_buf, vlk_atlas_img, 128, 32);
}
void vlk_save() {
  FILE * f = fopen("atlas.img", "wb");

  unsigned char * data;
  _(vkMapMemory(vlk_dev, vlk_atlas_h_mem, 0, VK_WHOLE_SIZE, 0, (void **)&data));
  fwrite(data, 128 * 32, 1, f);
  vkUnmapMemory(vlk_dev, vlk_atlas_h_mem);

  fclose(f);
}

#endif
#endif
