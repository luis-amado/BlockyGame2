#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 projection2D;

void main() {
  gl_Position = projection2D * model * vec4(aPos, 0.0, 1.0);
  texCoords = vec2(aPos.x, 1-aPos.y);
}