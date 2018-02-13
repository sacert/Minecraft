#version 330 core

in vec4 worldspace_position;

out vec4 outputColor;

const vec4 skytop = vec4(0.49f, 0.75f, 0.92f, 1.0f);
const vec4 skyhorizon = vec4(0.895f, 0.945f, 0.98f, 1.0f);

void main()
{
    vec4 pointOnSphere = vec4(normalize(worldspace_position.xyz), 1);
    float a = pointOnSphere.y;
    float i = 0.9;
    outputColor = i * mix(skyhorizon, skytop, a);
}