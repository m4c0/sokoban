#version 450
#extension GL_GOOGLE_include_directive : require
#include "metallic-floor.frag"

layout(push_constant) uniform upc {
  vec2 player_pos;
  float aspect;
  float time;
} pc;

layout(set = 0, binding = 0) uniform usampler2D u_map;

layout(location = 0) in vec2 q_pos;

layout(location = 0) out vec4 frag_color;

const float pi = 3.14159265358979323;

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}
float sd_rnd_box(vec2 p, vec2 b, float r) {
  return sd_box(p, b) - r;
}

float sd_cut_dist(vec2 p, float r, float h) {
  float w = sqrt(r * r - h * h);
  p.x = abs(p.x);
  float s = max((h - r) * p.x * p.x + w * w * (h + r - 2.0 * p.y),
                h * p.x - w * p.y);
  return (s < 0.0) ? length(p) - r : 
         (p.x < w) ? h - p.y :
         length(p - vec2(w, h));
}

vec4 brick(vec2 p) {
  vec2 b = p * vec2(12, 24);
  b.x += 0.5 * step(1.0, mod(b.y, 2));

  vec2 i = floor(b);
  float h = hash(i);
  h = smoothstep(0.2, 0.8, h) * 0.3 + 0.5;
  h = h * (noise(b) * 0.3 + 0.7);

  vec2 f = fract(b) - 0.5;
  float d = sd_box(f, vec2(0.5, 0.5));
  d = 1.0 - exp(-16.0 * abs(d));

  vec3 c = vec3(0.6, 0.3, 0.1) * h * d;

  return vec4(c, 1);
}

uvec4 map_at(vec2 p, vec2 d) {
  vec2 uv = p;
  uv = floor(d + uv * 24.0 - vec2(0, 8)) / 32.0;
  uv = uv * 0.5 + 0.5;
  uv = clamp(uv, 0, 1);
  return texture(u_map, uv);
}

float shadow_side(vec2 p, vec2 d, float b, float m) {
  uvec4 map = map_at(p, d);
  float f = (map.r == 88) ? 0.0 : 1.0;
  f = mix(f, 1.0, b); 
  return min(m, f);
}

float shadow(vec2 p) {
  vec2 b = fract(p * vec2(24));
  float m = 1.0f;

  m = shadow_side(p, vec2(-1, 0), b.x, m);
  m = shadow_side(p, vec2(+1, 0), 1.0 - b.x, m);
  m = shadow_side(p, vec2(0, -1), b.y, m);
  m = shadow_side(p, vec2(0, +1), 1.0 - b.y, m);

  m = shadow_side(p, vec2(-1, -1), max(b.x, b.y), m);
  m = shadow_side(p, vec2(-1, +1), max(b.x, 1.0 - b.y), m);
  m = shadow_side(p, vec2(+1, -1), max(1.0 - b.x, b.y), m);
  m = shadow_side(p, vec2(+1, +1), max(1.0 - b.x, 1.0 - b.y), m);

  return m;
}

vec4 outside(vec2 p) {
  float m = shadow(p);
  m = 1.0 - exp(-6.0 * abs(m));

  vec4 c = metal_floor(p);
  c.rgb = c.rgb * 0.2 * m;
  return c;
}

vec4 empty(vec2 p) {
  float m = shadow(p);

  float s = 1.0 - exp(-7.0 * abs(m));

  float csel = step(0.5, m);
  csel += 1.0 - step(0.3, m);

  const vec4 flr = vec4(0.05, 0.15, 0.1, 1.0);
  const vec4 ylw = vec4(0.5, 0.5, 0.1, 1.0) * 0.5;

  vec4 c = mix(ylw, flr, csel);
  c.rgb = c.rgb * s;
  return c;
}

vec4 target(vec2 p, vec2 b) {
  float d = length(b);
  d = exp(-d * d * 20) * sin(abs(d) * 30 - pc.time * 2);

  const vec3 t = vec3(4.0, 0.2, 0.1);

  vec3 c = empty(p).rgb;
  c = mix(c, t, d);
  return vec4(c, 1);
}

vec4 box(vec2 p, vec2 b, bool on_tgt) {
  float d = sd_rnd_box(b, vec2(0.25), 0.1);

  vec3 ins = on_tgt ? vec3(0.5, 0.2, 0.1) : vec3(0.1, 0.4, 0.5);
  ins *= 1.0 - 0.01 / (d * d);
  ins *= smoothstep(0.0, 0.2, fract(b.y * 5.0));

  vec3 brd = on_tgt ? vec3(1.0, 0.3, 0.1) : vec3(0.1, 0.7, 1.0);
  brd *= 0.1 * smoothstep(-0.1, 0.0, d) + 0.9;

  vec4 box = vec4(mix(ins, brd, step(-0.125, d)), 1.0);
  box *= 0.5 * hash(b) + 0.5;

  vec4 flr = on_tgt ? target(p, b) : empty(p);
  flr *= 0.5 * smoothstep(0.0, 0.1, d) + 0.5;

  return mix(box, flr, step(0, d));
}

vec4 player(vec2 p, vec4 c) {
  vec2 b = q_pos * vec2(12) + 12 - pc.player_pos + vec2(4.0, 0.0) - vec2(0.5);

  float hd_d = sd_circle(b + vec2(0.0, 0.2), 0.2);

  float bd_d = sd_cut_dist(vec2(0.0, 0.2) - b, 0.3, -0.2);

  float d = min(hd_d, bd_d);
  d = step(0, d);

  return mix(vec4(1), c, d);
}

void main() {
  uvec4 map = map_at(q_pos, vec2(0));
  vec2 b = fract(q_pos * vec2(12)) - 0.5;

  vec4 f;
  if (map.r == 88) { // 'X' - wall
    f = brick(q_pos);
  } else if (map.r == 32) { // ' ' - outside
    f = outside(q_pos);
  } else if (map.r == 42) { // '*' - target
    f = target(q_pos, b);
  } else if (map.r == 48) { // '0' - target_box
    f = box(q_pos, b, true);
  } else if (map.r == 79) { // 'O' - box
    f = box(q_pos, b, false);
  } else {
    f = empty(q_pos);
  }

  f = player(q_pos, f);

  frag_color = vec4(f.rgb, 1);
}
