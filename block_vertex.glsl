#version 330 core

uniform mat4 camera;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;

void main() {

    fragTexCoord = vertTexCoord;

    // does not alter the verticies at all
    gl_Position = camera * vec4(vert, 1);
}