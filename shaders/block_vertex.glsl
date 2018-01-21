#version 330 core

uniform float selected;
uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;
out vec3 vertCoord;

void main() {

    fragTexCoord = vertTexCoord;
    vertCoord = vert;

    // does not alter the verticies at all
    gl_Position = camera * model * vec4(vert, 1);
}