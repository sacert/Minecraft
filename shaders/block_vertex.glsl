#version 330 core

uniform float selected;
uniform mat4 camera;
uniform mat4 view;

in vec3 vert;
in vec2 vertTexCoord;
layout(location = 2) in vec3 vertNormal;

out vec2 fragTexCoord;
out vec3 vertCoord;

out vec3 normal;

void main() {

    fragTexCoord = vertTexCoord;
    vertCoord = vert;
    normal = vertNormal;

    // does not alter the verticies at all
    gl_Position = camera * vec4(vert, 1);
}