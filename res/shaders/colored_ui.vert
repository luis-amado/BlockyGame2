#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 projection2D;

void main() {
  gl_Position = projection2D * model * vec4(aPos, 0.0, 1.0);
}