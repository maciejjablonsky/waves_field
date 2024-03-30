#version 420 core
layout(location = 0) in vec3 in_position;

out vec3 frag_position;
out vec3 normal;

layout(std140, binding = 0) uniform camera
{
    mat4 u_view;
    mat4 u_projection;
    vec3 u_camera_position;
};

layout(std140, binding = 2) uniform waves
{
    float u_amplitude[32];
    float u_frequency[32];
    float u_phase_0[32];
    float u_speed[32];
    vec3 u_direction[32];
};

uniform mat4 u_model;
uniform float u_duration;

float omega(float frequency)
{
	return 2 * 3.14159265359 * frequency;
}

void main()
{
	float waveYOffset = 0.0;
    float dWaveY_dx = 0.0; // Partial derivative with respect to x
    float dWaveY_dz = 0.0; // Partial derivative with respect to z

    for(int i = 0; i < 32; ++i) {
        float omega = omega(u_frequency[i]);
        float distance = dot(in_position, u_direction[i]);
        float phase_without_zero_phase = omega * u_duration + (omega * distance / u_speed[i]);
        float common_phase = u_phase_0[i] + phase_without_zero_phase;

          waveYOffset += u_amplitude[i] * (sin(common_phase) - 1.0);
          float common_derivative = u_amplitude[i] * cos(phase_without_zero_phase);
          dWaveY_dx += omega * u_direction[i].x *  common_derivative;
          dWaveY_dz += omega * u_direction[i].z *  common_derivative;

    }

    vec3 positionWithWave = vec3(in_position.x, in_position.y + waveYOffset, in_position.z);

    // Compute tangent and binormal
	vec3 T = normalize(vec3(1, 0, dWaveY_dx)); // Corrected to use a component of dWaveY_dx
	vec3 B = normalize(vec3(0, 1, dWaveY_dz)); // Corrected to use a component of dWaveY_dy


    frag_position = vec3(u_model * vec4(positionWithWave, 1.0));
    normal = normalize(cross(T, B));

    gl_Position = u_projection * u_view * u_model * vec4(positionWithWave, 1.0);
}
