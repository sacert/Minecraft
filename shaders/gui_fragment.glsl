#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

	color = texture( myTextureSampler, UV );

    // transparency in png becomes black
    // lame approach is remove alpha where it's transparent
    if (color.a <= 0) {
        discard;
    }
}
