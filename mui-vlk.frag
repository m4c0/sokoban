#version 450

layout(push_constant) uniform upc {
  vec4 rect;
  vec4 colour;
  vec4 uv;
} pc;

layout(binding = 1) uniform sampler2D txt;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

float sd_circle(vec2 p, float r) { return length(p) - r; }
float op_xor(float a, float b) { return max(min(a, b), -max(a, b)); }

float settings() {
  vec2 p = f_pos * 2 - 1;
  p /= 12;

  float dc = sd_circle(p, 0.035);
  float angle = atan(p.y / p.x);
  float gr = smoothstep(-0.6, 0.6, sin(angle * 8));
  float dg = sd_circle(p, mix(0.045, 0.06, gr));

  float d = op_xor(dc, dg);
  d = smoothstep(0.005, 0.0, d);
  return d;
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
