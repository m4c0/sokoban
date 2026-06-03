#version 450

layout(push_constant) uniform upc {
  vec4 rect;
  vec4 colour;
  vec4 uv;
} pc;

layout(binding = 0) uniform sampler2D txt;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

void main() {
  float a = mix(texture(txt, f_pos).r, 1, pc.colour.a);
  colour = vec4(pc.colour.rgb, a);
}
