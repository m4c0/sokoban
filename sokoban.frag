#version 450
#extension GL_GOOGLE_include_directive : require
#include "metallic-floor.frag"

layout(push_constant) uniform upc {
  float aspect;
} pc;

layout(set = 0, binding = 0) uniform sampler2D u_map;

layout(location = 0) in vec2 q_pos;

layout(location = 0) out vec4 frag_color;

void main() {
  vec2 uv = q_pos;
  uv = floor(uv * 24.0 - vec2(0, 8)) / 32.0;
  uv = uv * 0.5 + 0.5;

  vec4 map = texture(u_map, uv);
  float blk = map.r * 256.0f;

  vec4 f;
  if (blk == 88) { // 'X'
    f = vec4(1);
  } else {
    f = metal_floor(q_pos);
  }

  frag_color = f;
}
