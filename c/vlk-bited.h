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

static VkDescriptorPool      vlk_dpool;
static VkDescriptorSetLayout vlk_dsl;
static VkDescriptorSet       vlk_dset;
static VkPipelineLayout      vlk_pl;
static VkPipeline            vlk_ppl;
static VkSampler             vlk_smp;

static void vlk_record(VkCommandBuffer cb) {
  vkCmdPushConstants(cb, vlk_pl, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vlk_upc_t), &vlk_pc);
  vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_ppl);
  vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_pl, 0, 1, &vlk_dset, 0, NULL);
  vkCmdDraw(cb, 3, 1, 0, 0);
}

void vlk_init() {
  vlk_create();

  vlk_load_atlas(fopen("atlas.img", "rb"));

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

void vlk_deinit() {
  vkDeviceWaitIdle(vlk_dev);

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

  vlk_destroy();
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
