#version 330 core
layout(location = 0) in vec3 Position;

uniform mat4 posistion;

void main()
{
   gl_Position = posistion * vec4(Position, 1.0);
}
