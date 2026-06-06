#version 450

layout(push_constant) uniform upc {
  vec4 rect;
  vec4 colour;
  vec4 uv;
} pc;

layout(binding = 1) uniform sampler2D txt;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

float settings() {
  return 0;
}

float font() {
  vec2 p = pc.uv.xy + f_pos * pc.uv.zw;
  return texture(txt, p).r;
}

float alpha() {
  switch (int(pc.colour.a)) {
    default:     return 1;
    case 0:      return font();
    case 0xEE00: return settings();
  }
}

void main() {
  colour = vec4(pc.colour.rgb, alpha());
}
