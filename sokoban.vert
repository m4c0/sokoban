#version 450

layout(push_constant) uniform upc {
  float aspect;
} pc;

layout(location = 0) in vec2 pos;
layout(location = 0) out vec2 q_uv;

void main() {
  vec2 p = pos * 2.0 - 1.0;
  vec2 uv = q_uv = p * vec2(pc.aspect, 1);
  gl_Position = vec4(p, 0, 1);
}
