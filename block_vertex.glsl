#version 330 core

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;

void main() {

    fragTexCoord = vertTexCoord;

    // does not alter the verticies at all
    gl_Position = camera * model * vec4(vert, 1);
}