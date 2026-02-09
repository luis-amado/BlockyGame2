#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 projection2D;

void main() {

  vec4 worldPos = model * vec4(aPos.x, aPos.y, 0.0, 1.0);

  worldPos.xy = round(worldPos.xy);
  
  gl_Position = projection2D * worldPos;
  texCoords = vec2(aPos.x, 1.0 - aPos.y);
}