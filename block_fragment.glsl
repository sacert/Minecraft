#version 330 core

uniform sampler2D tex;
in vec2 fragTexCoord;
// out meaning to the video buffer
out vec4 frag_color;

void main(){
    vec4 color = texture(tex, fragTexCoord);

    // check if grass, change to green if so -- NOTE: Simplified version to change grass color, change depending on biome later
    // this is ugly and it works but yknow the old saying, "if it's ugly and it works, it's still ugly" :(
    if ( 0 <= fragTexCoord.x && fragTexCoord.x <= 0.0625 && 0 <= fragTexCoord.y && fragTexCoord.y <= 0.0625) {
        frag_color = vec4(color.r * 0.6, color.g*0.90, color.b * 0.3118, 1.0) * vec4(1.2,1.2,1.2,1.0);
    } else {
        frag_color = vec4(color.r, color.g, color.b, 1.0);
    }
}
