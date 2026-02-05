#version 330 core

layout (location = 0) in vec3 a_Pos;

uniform vec3 start;
uniform vec3 end;

uniform mat4 projection;
uniform mat4 view;

void main() {
  vec3 position = start;
  if (a_Pos.x > 0) {
    position = end;
  }
  gl_Position = projection * view * vec4(position, 1.0);
}