#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 worldspace_position;

void main()
{
    vec4 eyespace_position;
    vec4 raw_position;

    mat4 view_without_translation = view;
    view_without_translation[3][0] = 0.0f;
    view_without_translation[3][1] = 0.0f;
    view_without_translation[3][2] = 0.0f;

    raw_position = vec4(position, 1);
    worldspace_position = model * raw_position;
    eyespace_position =  view_without_translation * worldspace_position;

    gl_Position = (projection * eyespace_position).xyww;
}