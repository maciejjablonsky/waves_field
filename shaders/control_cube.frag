#version 330 core
out vec4 FragColor;
uniform vec3 cube_color;

void main() {
    FragColor = vec4(cube_color, 1.0); // Cube color
}
