#version 330 core

uniform sampler2D tex;
uniform vec3 selected;
in float select;

in vec2 fragTexCoord;
// out meaning to the video buffer
out vec4 frag_color;

in vec3 vertCoord;

void main(){
    vec4 color = texture(tex, fragTexCoord);

    if (selected.y != 999) {
        if ((vertCoord.x >= selected.x && vertCoord.x <= (selected.x + 1)) && (vertCoord.y >= selected.y && vertCoord.y <= (selected.y + 1)) && (vertCoord.z >= selected.z && vertCoord.z <= (selected.z + 1))) {
            if ((vertCoord.x < (selected.x+0.01) || vertCoord.x > (selected.x+0.99)) && (vertCoord.z < (selected.z+0.01) || vertCoord.z > (selected.z+0.99)) || 
            (vertCoord.y < (selected.y+0.01) || vertCoord.y > (selected.y+0.99)) && (vertCoord.z < (selected.z+0.01) || vertCoord.z > (selected.z+0.99)) ||
            (vertCoord.x < (selected.x+0.01) || vertCoord.x > (selected.x+0.99)) && (vertCoord.y < (selected.y+0.01) || vertCoord.y > (selected.y+0.99))) {

                frag_color = vec4(0.2, 0.2, 0.2, 1.0);
                return;
            }

        }
    }

    // check if grass, change to green if so -- NOTE: Simplified version to change grass color, change depending on biome later
    // this is ugly and it works but yknow the old saying, "if it's ugly and it works, it's still ugly" :(
    if ( 0 <= fragTexCoord.x && fragTexCoord.x <= 0.0625 && 0 <= fragTexCoord.y && fragTexCoord.y <= 0.0625) {
        frag_color = vec4(color.r * 0.6, color.g*0.90, color.b * 0.3118, 1.0) * vec4(1.2,1.2,1.2,1.0);
    } else {
        frag_color = vec4(color.r, color.g, color.b, 1.0);
    }
}