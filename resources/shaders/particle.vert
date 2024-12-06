#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aSize;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    gl_Position = projMatrix * viewMatrix * vec4(aPosition, 1.0);
    gl_PointSize = aSize;
}
