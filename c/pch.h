#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef VLK_USE_VOLK
#  include "volk.h"
#else
#  include "Vulkan-Headers/include/vulkan/vulkan_core.h"
#  ifdef __APPLE__
#    include "Vulkan-Headers/include/vulkan/vulkan_metal.h"
#  endif
#endif
