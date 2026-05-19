#version 450

layout(push_constant) uniform upc {
  vec2 grid_pos;
  vec2 grid_size;
} pc;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 i_pos;
layout(location = 2) in vec4 i_color;
layout(location = 3) in vec4 i_uv;
layout(location = 4) in vec4 i_mult;
layout(location = 5) in vec4 i_rot;

layout(location = 0) out vec4 q_color;
layout(location = 1) out vec2 q_uv;
layout(location = 2) out vec4 q_mult;

const float pi = 3.14159265358979323f;

void main() {
  const float gap = 0.00001f;

  q_color = i_color;
  q_uv = mix(i_uv.xy + gap, i_uv.zw - gap, pos);
  q_mult = i_mult;

  vec2 f_adj = pos * gap; // Avoids gaps in aligned grids

  float theta = i_rot.x * pi / 180.0f;
  const mat2 rot = mat2(
    cos(theta), -sin(theta),
    sin(theta), cos(theta)
  );

  vec2 p = pos * i_pos.zw;
  p -= i_rot.yz;
  p = rot * p;
  p += i_rot.yz;

  vec2 f_pos = (p + i_pos.xy - pc.grid_pos) / pc.grid_size; 
  gl_Position = vec4(f_pos + f_adj, 0, 1);
}
