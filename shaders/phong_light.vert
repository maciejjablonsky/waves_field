#version 420 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 frag_position;
out vec3 normal;

layout(std140, binding = 0) uniform camera
{
    mat4 u_view;
    mat4 u_projection;
    vec3 u_camera_position;
};

uniform mat4 u_model;

void main()
{
    frag_position = vec3(u_model * vec4(in_position, 1.0));
    normal        = in_normal;

    gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
}
