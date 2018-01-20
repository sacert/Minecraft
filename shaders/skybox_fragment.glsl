#version 330 core
in Data
{
    vec4 eyespace_position;
    vec4 eyespace_normal;
    vec4 worldspace_position;
    vec4 raw_position;
} vtx_data;

out vec4 outputColor;

const vec4 skytop = vec4(0.0f, 0.0f, 1.0f, 1.0f);
const vec4 skyhorizon = vec4(0.3294f, 0.92157f, 1.0f, 1.0f);

void main()
{
    vec4 pointOnSphere = vec4(normalize(vtx_data.worldspace_position.xyz), 1);
    float a = pointOnSphere.y;
    float i = 0.5;
    outputColor = i * mix(skyhorizon, skytop, a);
}