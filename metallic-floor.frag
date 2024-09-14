// Based on "Disco Floor" by andremichelle
// https://www.shadertoy.com/view/WsVXW3

float hash(vec2 p) {
  return fract(cos(dot(p, vec2(91.52, -74.27))) * 939.24);
}

float value(vec2 p) {
  vec2 f = floor(p);
  vec2 s = p - f;
  vec4 m = (s * s * (3. - s - s)).xyxy;
  m = m * vec4(-1, -1, 1, 1) + vec4(1, 1, 0, 0);
  return (hash(f + vec2(0, 0)) * m.x + hash(f + vec2(1, 0)) * m.z) * m.y +
    (hash(f + vec2(0, 1)) * m.x + hash(f + vec2(1, 1)) * m.z) * m.w;
}

float noise(vec2 p) {
  return
    .6 * value(p * 8.) +
    .4 * value(p * 16.) +
    .3 * value(p * 32.) +
    .2 * value(p * 64.);
}

float mat(vec2 uv, vec2 offset) { return noise(uv * vec2(6., 20.) + offset); }
float ndot(vec2 a, vec2 b) { return a.x * b.x - a.y * b.y; }

float sd_rhombus(vec2 p, vec2 b) {
  vec2 q = abs(p);
  float h = clamp((-2.0 * ndot(q, b) + ndot(b, b)) / dot(b, b), -1.0, 1.0);
  float d = length(q - 0.5 * b * vec2(1.0 - h, 1.0 + h));
  return d * sign(q.x * b.y + q.y * b.x - b.x * b.y);
}

float sd_circle(vec2 p, float r) { return length(p) - r; }

float op_smooth_union(float d1, float d2, float k) {
  float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
  return mix(d2, d1, h) - k * h * (1.0 - h);
}

float sd_diamond(vec2 p) {
  return op_smooth_union(sd_rhombus(p, vec2(.2, 1.)), sd_circle(p, .2), .1);
}

vec4 over(vec4 a, vec4 b) { return mix(a, b, 1. - a.w); }

mat2 rot(float a) {
  float c = cos(a);
  float s = sin(a);
  return mat2(c, -s, s, c);
}

vec4 diamonds(vec2 uv, float angle) {
  vec2 p = vec2(uv);
  mat2 r = rot(angle);
  p = fract(p) * 2. - 1.;
  p *= r;

  float d = sd_diamond(p);
  float a = smoothstep(.01, .0, d);
  float zp = smoothstep(.0, .1, -d);
  float zh = smoothstep(.0, .1, -sd_diamond(p + vec2(.01, .0)));
  float zv = smoothstep(.0, .1, -sd_diamond(p + vec2(.0, .01)));
  return zp + zh + zv == 0. 
    ? vec4(0, 0, 0, a)
    : vec4(normalize(vec3(zp - zh, zp - zv, zp)), a);	
}

vec3 light(vec3 lp, vec3 lc, vec3 n, vec2 uv) {
  vec3 ld = vec3(lp.xy - uv, lp.z - n.z);
  float l = length(ld);
  return lc * pow(max(.0, dot(n, ld / l)), 4.) / l;
}

vec4 metal_floor(vec2 uv) {
  uv = uv * 5.0;

  float n0 = mat(uv, vec2(.0));
  float n1 = mat(uv, vec2(.001, .000));
  float n2 = mat(uv, vec2(.000, .001));
    
  vec4 c = vec4(.0);
  vec4 d0 = diamonds(uv * 3. + .0,  .78539);
  vec4 d1 = diamonds(uv * 3. + .5, -.78539);
  float a = max(d0.w, d1.w);
  vec3 n = (d0.xyz + d1.xyz) * a + vec3(n0 - n1, n0 - n2, n0) * .6 * (1. - a * .6);
  float t = 3.7f;
  mat2 m0 = rot( t *1.4 + .4) * sin(t * .4 + .2);
  mat2 m1 = rot(-t *1.4 + .7) * sin(t * .6 + .5);
  mat2 m2 = rot( t *1.4 + .9) * cos(t * .8 + .9);

  // light colors
  vec3 l0c = vec3(1., .3, 1.);
  vec3 l1c = vec3(.1, .3, 1.);
  vec3 l2c = vec3(.1, 1., 1.);
  // light positions
  vec3 l0v = vec3(vec2(.99, 0.) * m0, 1.5);
  vec3 l1v = vec3(vec2(.70, 0.) * m1, 1.5);
  vec3 l2v = vec3(vec2(.00, 1.) * m2, 1.5);
  // light intensity
  float li0 = pow(distance(uv, l0v.xy), -1.4);
  float li1 = pow(distance(uv, l1v.xy), -1.4);
  float li2 = pow(distance(uv, l2v.xy), -1.4);

  vec3 rc = vec3(.05 + n0 * n0 * n0 * n0 * .1);
  rc += pow(max(.0, n.z), 2.) * .12;
  rc += light(l0v, l0c, n, uv);
  rc += light(l1v, l1c, n, uv);
  rc += light(l2v, l2c, n, uv);
  return over(vec4(rc, 1.), c);
}
