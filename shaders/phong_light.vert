#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 frag_position;
out vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    frag_position = vec3(u_model * vec4(in_position, 1.0));
    normal = in_normal;
    
    gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
}
