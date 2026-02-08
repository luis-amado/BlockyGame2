#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 color;

void main() {
  gl_Position = projection * view * model * vec4(a_Pos, 1.0);
  color = a_Color; 
}