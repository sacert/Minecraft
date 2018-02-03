#version 330 core

uniform float selected;
uniform mat4 camera;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;
out vec3 vertCoord;

void main() {

    fragTexCoord = vertTexCoord;
    vertCoord = vert;

    // does not alter the verticies at all
    gl_Position = camera * vec4(vert, 1);
}