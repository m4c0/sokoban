#version 450
#extension GL_GOOGLE_include_directive : require
#include "metallic-floor.frag"

layout(push_constant) uniform upc {
  float aspect;
} pc;

layout(set = 0, binding = 0) uniform usampler2D u_map;

layout(location = 0) in vec2 q_pos;

layout(location = 0) out vec4 frag_color;

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

vec4 brick(vec2 p) {
  vec2 b = p * vec2(12, 24);
  b.x += 0.5 * step(1.0, mod(b.y, 2));

  vec2 i = floor(b);
  float h = hash(i);
  h = smoothstep(0.2, 0.8, h) * 0.3 + 0.5;

  vec2 f = fract(b) - 0.5;
  float d = sd_box(f, vec2(0.5, 0.5));
  d = 1.0 - exp(-16.0 * abs(d));

  vec3 c = vec3(h * d);

  return vec4(c, 1);
}

void main() {
  vec2 uv = q_pos;
  uv = floor(uv * 24.0 - vec2(0, 8)) / 32.0;
  uv = uv * 0.5 + 0.5;
  uv = clamp(uv, 0, 1);

  uvec4 map = texture(u_map, uv);

  vec4 f;
  if (map.r == 88) { // 'X'
    f = brick(q_pos);
  } else {
    f = metal_floor(q_pos);
  }

  frag_color = f;
}
