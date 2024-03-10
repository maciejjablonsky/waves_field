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

layout(std140, binding = 1) uniform waves
{
    float u_amplitude[32];
    float u_frequency[32];
    float u_phase_0[32];
    float u_speed[32];
    vec3 u_direction[32];
    float u_duration[32];
};

uniform mat4 u_model;

void main()
{
    // frag_position = vec3(u_model * vec4(in_position, 1.0));
    // normal        = in_normal;

    // gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
    float waveYOffset = 0.0;
    for(int i = 0; i < 32; ++i) {
        // Calculate wave effect based on sine function
        float phase = u_phase_0[i] + u_speed[i] * u_duration[i];
        waveYOffset += u_amplitude[i] * sin(u_frequency[i] * dot(u_direction[i].xy, in_position.xy) + phase);
    }

    vec3 positionWithWave = vec3(in_position.x, in_position.y + waveYOffset, in_position.z);

    frag_position = vec3(u_model * vec4(positionWithWave, 1.0));
    normal        = in_normal;

    gl_Position = u_projection * u_view * u_model * vec4(positionWithWave, 1.0);
}
