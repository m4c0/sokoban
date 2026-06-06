#ifndef VLK_GAME_H
#define VLK_GAME_H

void vlk_init();
void vlk_frame();
void vlk_deinit();

void vlk_update_map();
void vlk_overlay(int on);

#ifdef VLK_IMPL
#include "gme.h"
#include "lvl.h"
#include "mui.h"
#include "sfx.h"
#include "snd.h"
#include "skb.h"
#include "tim.h"
#include "vlk.h"

// Almost sure there is a way to do this with some Vulkan call
#ifdef __APPLE__
#  define VLK_EXT_SCALE 0.5f // Retina
#else
#  define VLK_EXT_SCALE 1.0f
#endif

typedef struct vlk_upc {
  float sel_rect_x, sel_rect_y, sel_rect_w, sel_rect_h;
  float player_pos_x, player_pos_y;
  float label_pos_x, label_pos_y;
  float cursor_x, cursor_y;
  float level;
  float aspect;
  float time;
  float overlay;
  float back_btn_dim;
  float menu_btn_dim;
} vlk_upc_t;

typedef struct vlk_mui_upc {
  float rect[4];
  float colour[4];
  float uv[4];
  float extent[2];
} vlk_mui_upc_t;

static vlk_upc_t vlk_pc;
static vlk_img_t vlk_map;

static VkDescriptorPool      vlk_dpool;
static VkDescriptorSetLayout vlk_dsl;
static VkDescriptorSet       vlk_dset;
static VkPipelineLayout      vlk_pl;
static VkPipeline            vlk_ppl;
static VkSampler             vlk_smp;

static VkPipelineLayout      vlk_mui_pl;
static VkPipeline            vlk_mui_ppl;

void vlk_overlay(int on) {
  vlk_pc.overlay = on ? 0.3 : 0.0;
}

static void uv(float * uv, char c) {
  if (c >= 'A' && c <= 'Z') c |= 0x20;
  if (c >= 'a' && c <= 'z') {
    float u = 32;
    for (char cc = 'a'; cc < c; cc++) u += mui_font_width(cc) + 1;

    uv[0] = u / 128.f;
    uv[1] = 1.f / 32.f;
    uv[2] = mui_font_width(c) / 128.f;
    uv[3] = mui_font_height() / 32.f;
    return;
  }

  uv[0] = uv[1] = uv[2] = uv[3] = 0;
}

static void vlk_record(VkCommandBuffer cb) {
  vlk_pc.cursor_x = vlk_pc.cursor_y = 10000;
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

  vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_mui_ppl);
  vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, vlk_mui_pl, 0, 1, &vlk_dset, 0, NULL);

  float sw = vlk_ext.width  * VLK_EXT_SCALE;
  float sh = vlk_ext.height * VLK_EXT_SCALE;
  skb_api->ui(sw, sh);

  mu_Command * cmd = NULL;
  while (mu_next_command(&mui_ctx, &cmd)) {
    switch (cmd->type) {
      case MU_COMMAND_TEXT: {
        vlk_mui_upc_t pc = {
          .rect   = { cmd->text.pos.x, cmd->text.pos.y, 0, mui_font_height() * 3 },
          .colour = {
            cmd->text.color.r / 255.f,
            cmd->text.color.g / 255.f,
            cmd->text.color.b / 255.f,
            0,
          },
          .extent = { sw, sh },
        };
        vkCmdPushConstants(cb, vlk_mui_pl, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(vlk_mui_upc_t), &pc);

        for (char * c = cmd->text.str; *c; c++) {
          pc.rect[2] = mui_font_width(*c) * 3;
          uv(pc.uv, *c);

          vkCmdPushConstants(cb, vlk_mui_pl, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(vlk_mui_upc_t), &pc);
          vkCmdDraw(cb, 4, 1, 0, 0);
          pc.rect[0] += pc.rect[2] + 2;
        }
        break;
      }
      case MU_COMMAND_CLIP: {
        VkRect2D rect = {
          { cmd->clip.rect.x, cmd->clip.rect.y },
          { cmd->clip.rect.w, cmd->clip.rect.h },
        };
        vkCmdSetScissor(cb, 0, 1, &rect);
        break;
      }
      case MU_COMMAND_RECT: {
        vlk_mui_upc_t pc = {
          .rect   = {
            cmd->rect.rect.x,
            cmd->rect.rect.y,
            cmd->rect.rect.w,
            cmd->rect.rect.h,
          },
          .colour = {
            cmd->rect.color.r / 255.f,
            cmd->rect.color.g / 255.f,
            cmd->rect.color.b / 255.f,
            1,
          },
          .extent = { sw, sh },
        };
        vkCmdPushConstants(cb, vlk_mui_pl, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(vlk_mui_upc_t), &pc);
        vkCmdDraw(cb, 4, 1, 0, 0);
        break;
      }
      case MU_COMMAND_ICON:
        break;
    }
  }
}

void vlk_update_map() {
  vlk_record_buf2img(vlk_map.h_buf, vlk_map.img, LVL_WIDTH, LVL_WIDTH);
}
static void vlk_load_map(int lvl) {
  lvl_load(lvl, gme_map);
  vlk_update_map();
}

static void vlk_mui_create_pipeline_layout() {
  VkPipelineLayoutCreateInfo pl_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount         = 1,
    .pSetLayouts            = &vlk_dsl,
    .pushConstantRangeCount = 1,
    .pPushConstantRanges = (VkPushConstantRange[]) {{
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .size       = sizeof(vlk_mui_upc_t),
    }},
  };
  _(vkCreatePipelineLayout(vlk_dev, &pl_info, NULL, &vlk_mui_pl));
}
static void vlk_mui_create_pipeline() {
  VkShaderModule vert = vlk_create_shader_module("mui-vlk.vert");
  VkShaderModule frag = vlk_create_shader_module("mui-vlk.frag");

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
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
        .blendEnable         = 1,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
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
    .layout     = vlk_mui_pl,
    .renderPass = vlk_rp,
  };
  _(vkCreateGraphicsPipelines(vlk_dev, NULL, 1, &ppl_info, NULL, &vlk_mui_ppl));

  vkDestroyShaderModule(vlk_dev, vert, NULL);
  vkDestroyShaderModule(vlk_dev, frag, NULL);
}
static void vlk_mui_init() {
  vlk_mui_create_pipeline_layout();
  vlk_mui_create_pipeline();
}
static void vlk_mui_deinit() {
  vkDestroyPipeline      (vlk_dev, vlk_mui_ppl, NULL);
  vkDestroyPipelineLayout(vlk_dev, vlk_mui_pl,  NULL);
}

void vlk_init() {
  vlk_create();

  vlk_create_img(&vlk_map, LVL_WIDTH, LVL_WIDTH, VK_FORMAT_R8_UINT);
  _(vkMapMemory(vlk_dev, vlk_map.h_mem, 0, VK_WHOLE_SIZE, 0, (void **)&gme_map));

  lvl_init(fopen("levels.txt", "r+"));

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

  vlk_mui_init();

  mui_init();
  snd_init(&sfx_filler);
  skb_init();
}

void vlk_deinit() {
  vkDeviceWaitIdle(vlk_dev);

  vkDestroySampler             (vlk_dev, vlk_smp,   NULL);
  vkDestroyDescriptorSetLayout (vlk_dev, vlk_dsl,   NULL);
  vkDestroyDescriptorPool      (vlk_dev, vlk_dpool, NULL);
  vkDestroyPipeline            (vlk_dev, vlk_ppl,   NULL);
  vkDestroyPipelineLayout      (vlk_dev, vlk_pl,    NULL);

  vlk_mui_deinit();
  vlk_destroy_img(&vlk_map);
  vlk_destroy();

  snd_deinit();
}

#endif
#endif
