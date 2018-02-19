#version 330 core

uniform sampler2D tex;
uniform vec3 selected;
in float select;

in vec2 fragTexCoord;
// out meaning to the video buffer
out vec3 frag_color;

in vec3 vertCoord;
in vec3 normal;

const vec3 light_direction = normalize(vec3(1, 1, 1));
const vec3 light_color = vec3(0.6);
const vec3 ambient = vec3(0.6);


void main(){
    vec4 color = texture(tex, fragTexCoord);

    // transparency in png becomes black
    // lame approach is remove alpha where it's transparent
    if (color.a <= 0) {
        discard;
    }

    if (selected.y != 999) {
        if ((vertCoord.x >= selected.x && vertCoord.x <= (selected.x + 1)) && (vertCoord.y >= selected.y && vertCoord.y <= (selected.y + 1)) && (vertCoord.z >= selected.z && vertCoord.z <= (selected.z + 1))) {
            if ((vertCoord.x < (selected.x+0.01) || vertCoord.x > (selected.x+0.99)) && (vertCoord.z < (selected.z+0.01) || vertCoord.z > (selected.z+0.99)) || 
            (vertCoord.y < (selected.y+0.01) || vertCoord.y > (selected.y+0.99)) && (vertCoord.z < (selected.z+0.01) || vertCoord.z > (selected.z+0.99)) ||
            (vertCoord.x < (selected.x+0.01) || vertCoord.x > (selected.x+0.99)) && (vertCoord.y < (selected.y+0.01) || vertCoord.y > (selected.y+0.99))) {

                frag_color = vec3(0.2, 0.2, 0.2);
                return;
            }
        }
    }

    // check if grass or leaves, change to green if so -- NOTE: Simplified version to change grass color, change depending on biome later
    // this is ugly and it works but yknow the old saying, "if it's ugly and it works, it's still ugly" :(
    if ( 0 <= fragTexCoord.x && fragTexCoord.x <= 0.0625 && 0 <= fragTexCoord.y && fragTexCoord.y <= 0.0625 ) { // grass block
        frag_color = vec3(color.r * 0.5, color.g*0.80, color.b * 0.26118) * vec3(1.2,1.2,1.2);
    } else if ( 0.25 <= fragTexCoord.x && fragTexCoord.x <= 0.3125 && 0.1875 <= fragTexCoord.y && fragTexCoord.y <= 0.25) { // leaves
        frag_color = vec3(color.r * 0.4, color.g*0.60, color.b * 0.16118) * vec3(1.2,1.2,1.2);
    } else if ( 0.4375 <= fragTexCoord.x && fragTexCoord.x <= 0.5 && 0.125 <= fragTexCoord.y && fragTexCoord.y <= 0.1875) { // grass plant
        frag_color = vec3(color.r * 0.4, color.g*0.8, color.b * 0.3) * vec3(1.2,1.2,1.2);
    } else {
        frag_color = vec3(color.r, color.g, color.b);
    }

    float diffuse = max(0, dot(normal, light_direction));
    vec3 light = ambient + vec3(1.0) * diffuse;
    frag_color = min(frag_color * light, vec3(1));

}