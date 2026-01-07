#version 330 core

out vec4 fragColor;
in vec2 texCoords;
in vec2 light;

uniform sampler2D tex;
uniform bool nightVision;

float ambientLighting = 0.1;

void main() {

  float maxLight = max(light.x, light.y);
  
  if (nightVision) {
    ambientLighting = 0.8;
  }
  float lightLevel = (maxLight * (1 - ambientLighting)) + ambientLighting;
  vec4 texColor = texture(tex, texCoords);

  if (texColor.a < 0.5) discard;

  fragColor = vec4(texColor.xyz * lightLevel, texColor.a);
}