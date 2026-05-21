#version 450

layout(push_constant) uniform upc {
  ivec2 cursor;
} pc;

layout(binding = 0) uniform sampler2D atlas;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

void main() {
  ivec2 p = ivec2(f_pos * vec2(128, 32));

  float r = texture(atlas, f_pos).r;
  float g = p == pc.cursor ? 1-r : r;
  colour = vec4(r, g, r, 1);
}
