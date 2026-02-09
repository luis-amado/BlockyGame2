#version 330 core

uniform sampler2D tex;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  fragColor = texture(tex, texCoords);
  return;
  if (texCoords.x < 0.01 || texCoords.x > 0.99) {
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  } else if (texCoords.y < 0.01 || texCoords.y > 0.99) {
    fragColor = vec4(1.0, 0.0, 1.0, 1.0);
  } else {
    fragColor = vec4(texCoords, 0.0, 1.0);
  }
}