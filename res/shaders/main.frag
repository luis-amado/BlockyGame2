#version 330 core

out vec4 fragColor;
in vec2 texCoords;
in float light;

uniform sampler2D tex;
uniform bool nightVision;

float ambientLighting = 0.1;

void main() {
  
  if (nightVision) {
    ambientLighting = 0.8;
  }
  float lightLevel = (light * (1 - ambientLighting)) + ambientLighting;
  vec4 texColor = texture(tex, texCoords);

  if (texColor.a < 0.5) discard;

  fragColor = vec4(texColor.xyz * lightLevel, texColor.a);
}