#version 450
#extension GL_GOOGLE_include_directive : require
#include "metallic-floor.frag"

layout(location = 0) out vec4 frag_color;

void main() {
  frag_color = vec4(0, 0, 1, 1);
}
