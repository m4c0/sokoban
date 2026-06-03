#version 450

layout(push_constant) uniform upc {
  vec4 rect;
  vec4 colour;
  vec4 uv;
  vec2 extent;
} pc;

layout(location = 0) out vec2 f_pos;

void main() {
  vec2 p = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);

  f_pos = pc.uv.xy + p * pc.uv.zw;

  p = pc.rect.xy + (p * pc.rect.zw);
  p /= pc.extent;

  gl_Position = vec4(p * 2 - 1, 0, 1);
}
