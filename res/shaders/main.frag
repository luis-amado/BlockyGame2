#version 330 core

out vec4 fragColor;
in vec2 texCoords;
in float light;

uniform sampler2D tex;

float ambientLighting = 0.2;

void main() {
  float lightLevel = (light * (1 - ambientLighting)) + ambientLighting;
  fragColor = texture(tex, texCoords) * lightLevel;
}