#version 420 core
out vec4 out_frag_color;

in vec3 frag_position;
in vec3 normal;

layout(std140, binding = 0) uniform camera
{
    mat4 u_view;
    mat4 u_projection;
    vec3 u_camera_position;
};

layout(std140, binding = 1) uniform phong_lighting
{
    uniform vec3 u_light_position;
    uniform vec3 u_light_color;
    uniform vec3 u_object_color;
};

void main()
{
    // ambient
    float ambientStrength = 0.6;
    vec3 ambient          = ambientStrength * u_light_color;

    // diffuse
    vec3 norm     = normalize(normal);
    vec3 lightDir = normalize(u_light_position - frag_position);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * u_light_color;

    float specularStrength = 0.9;
    vec3 viewDir           = normalize(u_camera_position - frag_position);
    vec3 reflectDir        = reflect(-lightDir, norm);
    float spec             = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular          = specularStrength * spec * u_light_color;

    vec3 result    = (ambient + diffuse + specular) * u_object_color;
    out_frag_color = vec4(result, 1.0);
}
