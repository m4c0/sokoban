#version 450

layout(location = 0) in vec2 pos;
layout(location = 0) out vec2 q_uv;

void main() {
  vec2 uv = q_uv = pos * 2.0f - 1.0f;
  gl_Position = vec4(uv, 0, 1);
}
