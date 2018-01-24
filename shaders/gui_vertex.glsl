#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 vert;
in vec2 vertTexCoord;

uniform vec2 scale;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){

    // UV of the vertex. No special space for this one.
	UV = vertTexCoord;

    vec4 vertScale = vec4(vec3(scale.x, scale.y, 0), 1);

    gl_Position = vertScale * vec4(vert,1);
	
}