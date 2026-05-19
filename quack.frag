#version 450

layout(push_constant) uniform upc {
  vec2 grid_pos;
  vec2 grid_size;
} pc;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) in vec4 i_color;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_mult;

layout(location = 0) out vec4 frag_color;

void main() {
  vec4 tex_color = texture(tex, i_uv) * i_mult;
  vec4 mix_color = mix(i_color, tex_color, tex_color.a);
  frag_color = mix_color;
}
