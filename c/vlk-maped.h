#ifndef VLK_GAME_H
#define VLK_GAME_H

void vlk_init();
void vlk_frame();
void vlk_deinit();

void vlk_load_next_level();
void vlk_load_prev_level();

#ifdef VLK_IMPL
#include "lvl.h"
#include "tim.h"
#include "vlk.h"

typedef struct vlk_vec2 {
  float x, y;
} vlk_vec2_t;
typedef struct vlk_vec4 {
  float x, y, z, w;
} vlk_vec4_t;
typedef struct vlk_upc {
  vlk_vec4_t sel_rect;
  float player_pos_x, player_pos_y;
  float label_pos_x, label_pos_y;
  vlk_vec2_t menu_size;
  float level;
  float aspect;
  float time;
  float back_btn_dim;
  float menu_btn_dim;
} vlk_upc_t;

static vlk_upc_t vlk_pc;
static vlk_img_t vlk_map;

static VkDescriptorPool      vlk_dpool;
static VkDescriptorSetLayout vlk_dsl;
static VkDescriptorSet       vlk_dset;
static VkPipelineLayout      vlk_pl;
static VkPipeline            vlk_ppl;
static VkSampler             vlk_smp;

static void vlk_record(VkCommandBuffer cb) {
  vlk_pc.label_pos_x = lvl_min_x;
  vlk_pc.label_pos_y = lvl_min_y - 1;
  vlk_pc.player_pos_x = lvl_px;
  vlk_pc.player_pos_y = lvl_py;
  vlk_pc.level = lvl_current + 1;
  vlk_pc.aspect = (float)vlk_ext.width / (float)vlk_ext.height;
  vlk_pc.time = tim_now();

  vkCmdPushConstants(cb, vlk_pl, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vlk_upc_t), &vlk_pc);
  vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_ppl);
  vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_pl, 0, 1, &vlk_dset, 0, NULL);
  vkCmdDraw(cb, 3, 1, 0, 0);
}

static void vlk_load_map(int lvl) {
  char * map;
  _(vkMapMemory(vlk_dev, vlk_map.h_mem, 0, VK_WHOLE_SIZE, 0, (void **)&map));
  lvl_load(lvl, map);
  vkUnmapMemory(vlk_dev, vlk_map.h_mem);

  vlk_record_buf2img(vlk_map.h_buf, vlk_map.img, LVL_WIDTH, LVL_WIDTH);
}

void vlk_init() {
  vlk_create();
  vlk_create_img(&vlk_map, LVL_WIDTH, LVL_WIDTH, VK_FORMAT_R8_UINT);

  lvl_init(fopen("levels.txt", "r"));

  vlk_load_atlas(vlk_open("atlas", "img"));
  vlk_load_map(0);

  VkDescriptorSetLayoutCreateInfo dsl_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .bindingCount = 2,
    .pBindings = (VkDescriptorSetLayoutBinding[]) {{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    }, {
      .binding = 1,
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
      .descriptorCount = 2,
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

  VkWriteDescriptorSet wds[] = {{
    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .dstSet          = vlk_dset,
    .dstBinding      = 0,
    .descriptorCount = 1,
    .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo      = (VkDescriptorImageInfo[]) {{
      .sampler       = vlk_smp,
      .imageView     = vlk_map.iv,
      .imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    }},
  }, {
    .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .dstSet          = vlk_dset,
    .dstBinding      = 1,
    .descriptorCount = 1,
    .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .pImageInfo      = (VkDescriptorImageInfo[]) {{
      .sampler       = vlk_smp,
      .imageView     = vlk_atlas.iv,
      .imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    }},
  }};
  vkUpdateDescriptorSets(vlk_dev, 2, wds, 0, NULL);

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

  VkShaderModule vert = vlk_create_shader_module("sokoban.vert");
  VkShaderModule frag = vlk_create_shader_module("sokoban.frag");

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

  vkDestroySampler             (vlk_dev, vlk_smp,   NULL);
  vkDestroyDescriptorSetLayout (vlk_dev, vlk_dsl,   NULL);
  vkDestroyDescriptorPool      (vlk_dev, vlk_dpool, NULL);
  vkDestroyPipeline            (vlk_dev, vlk_ppl,   NULL);
  vkDestroyPipelineLayout      (vlk_dev, vlk_pl,    NULL);

  vlk_destroy_img(&vlk_map);
  vlk_destroy();
}

void vlk_load_next_level() {
  int lvl = lvl_current + 1;
  if (lvl > 59) lvl = 59;
  vlk_load_map(lvl);
}
void vlk_load_prev_level() {
  int lvl = lvl_current - 1;
  if (lvl < 0) lvl = 0;
  vlk_load_map(lvl);
}

#endif
#endif
