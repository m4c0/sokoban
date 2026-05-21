#version 450

layout(binding = 0) uniform sampler2D atlas;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

void main() {
  colour = texture(atlas, f_pos).rrrr;
}
