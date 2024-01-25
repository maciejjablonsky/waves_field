#version 330 core
out vec4 FragColor;

uniform vec3 axis_color;

void main() {
    FragColor = vec4(axis_color, 1.0);
}