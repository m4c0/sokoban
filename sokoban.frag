#version 450
#extension GL_GOOGLE_include_directive : require
#include "metallic-floor.frag"

layout(set = 0, binding = 0) uniform sampler2D map;

layout(location = 0) in vec2 q_uv;
layout(location = 0) out vec4 frag_color;

void main() {
  vec4 f = metal_floor(q_uv);

  vec2 map_uv = q_uv * 0.5 + 0.5;
  map_uv = floor(map_uv * 24) + 0.5;
  map_uv /= 32.0;
  vec4 map = texture(map, map_uv);

  vec2 m = floor(q_uv * 12);
  float mf = mod(m.x + m.y, 2.0);
  f = mix(f, vec4(0), mf);
  
  frag_color = f;
}
