#version 450

layout(push_constant) uniform upc {
  vec4 sel_rect;
  vec2 player_pos;
  vec2 label_pos;
  vec2 menu_size;
  float level;
  float aspect;
  float time;
  float back_btn_dim;
  float menu_btn_dim;
} pc;

layout(location = 0) in vec2 pos;

layout(location = 0) out vec2 q_pos;

void main() {
  vec2 p = pos * 2.0 - 1.0;
  q_pos = p * vec2(pc.aspect, 1);
  gl_Position = vec4(p, 0, 1);
}
