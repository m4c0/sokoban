#version 450

layout(location = 0) in vec2 pos;

void main() {
  gl_Position = vec4(pos * 2.0f - 1.0f, 0, 1);
}
