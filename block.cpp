#include "block.h"
#include <iostream>

TextureSides sameSides(int x, int y) {
    TextureSides ts;

    ts.bottom_x = x;
    ts.bottom_y = y;
    ts.top_x = x;
    ts.top_y = y;
    ts.right_x = x;
    ts.right_y = y;
    ts.left_x = x;
    ts.left_y = y;
    ts.front_x = x;
    ts.front_y = y;
    ts.back_x = x;
    ts.back_y = y;

    return ts;
}

TextureSides differentSides(int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, 
    int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y) {
    TextureSides ts;

    ts.bottom_x = bottom_x;
    ts.bottom_y = bottom_y;
    ts.top_x = top_x;
    ts.top_y = top_y;
    ts.right_x = right_x;
    ts.right_y = right_y;
    ts.left_x = left_x;
    ts.left_y = left_y;
    ts.front_x = front_x;
    ts.front_y = front_y;
    ts.back_x = back_x;
    ts.back_y = back_y;

    return ts;
}

TextureSides getTextureSides(BlockType bt) {

    TextureSides gts;

    if (bt == 1) {              // Grass
        gts = differentSides( 3, 15, 3, 15, 3, 15, 3, 15, 0, 15, 2, 15);
    } else if (bt == 2) {       // Dirt
        gts = sameSides(2, 15);
    } else if (bt == 3) {       // Cobblestone
        gts = sameSides(0, 14);
    } else if (bt == 4) {       // Bedrock
        gts = sameSides(1, 14);
    }

    return gts;
}

void addBottomFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x, coord.y, coord.z,   TEXCOORDX(ts.bottom_x),  TEXCOORDY(ts.bottom_y), 0, -1, 0,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.bottom_x)+N,TEXCOORDY(ts.bottom_y), 0, -1, 0,
        coord.x, coord.y, coord.z+1.0f,   TEXCOORDX(ts.bottom_x),  TEXCOORDY(ts.bottom_y)+N, 0, -1, 0,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.bottom_x)+N,TEXCOORDY(ts.bottom_y), 0, -1, 0,
        coord.x+1.0f, coord.y, coord.z+1.0f,   TEXCOORDX(ts.bottom_x)+N,TEXCOORDY(ts.bottom_y)+N, 0, -1, 0,
        coord.x, coord.y, coord.z+1.0f,   TEXCOORDX(ts.bottom_x),  TEXCOORDY(ts.bottom_y)+N, 0, -1, 0,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}

void addTopFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x, coord.y+1.0f, coord.z,   TEXCOORDX(ts.top_x),  TEXCOORDY(ts.top_y), 0, 1, 0,
        coord.x, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.top_x),  TEXCOORDY(ts.top_y)+N, 0, 1, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z,   TEXCOORDX(ts.top_x)+N,TEXCOORDY(ts.top_y), 0, 1, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z,   TEXCOORDX(ts.top_x)+N,TEXCOORDY(ts.top_y), 0, 1, 0,
        coord.x, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.top_x),  TEXCOORDY(ts.top_y)+N,  0, 1, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.top_x)+N,TEXCOORDY(ts.top_y)+N, 0, 1, 0,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}

void addFrontFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x, coord.y, coord.z+1.0f,   TEXCOORDX(ts.front_x)+N,TEXCOORDY(ts.front_y), 0, 0, -1,
        coord.x+1.0f, coord.y, coord.z+1.0f,   TEXCOORDX(ts.front_x),  TEXCOORDY(ts.front_y), 0, 0, -1,
        coord.x, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.front_x)+N,TEXCOORDY(ts.front_y)+N, 0, 0, -1,
        coord.x+1.0f, coord.y, coord.z+1.0f,   TEXCOORDX(ts.front_x),  TEXCOORDY(ts.front_y), 0, 0, -1,
        coord.x+1.0f, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.front_x),  TEXCOORDY(ts.front_y)+N, 0, 0, -1,
        coord.x, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.front_x)+N,TEXCOORDY(ts.front_y)+N, 0, 0, -1,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}

void addBackFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x, coord.y, coord.z,   TEXCOORDX(ts.back_x),  TEXCOORDY(ts.back_y), 0, 0, 1,
        coord.x, coord.y+1.0f, coord.z,   TEXCOORDX(ts.back_x),  TEXCOORDY(ts.back_y)+N,  0, 0, 1,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.back_x)+N,TEXCOORDY(ts.back_y),  0, 0, 1,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.back_x)+N,TEXCOORDY(ts.back_y),  0, 0, 1,
        coord.x, coord.y+1.0f, coord.z,   TEXCOORDX(ts.back_x),  TEXCOORDY(ts.back_y)+N,  0, 0, 1,
        coord.x+1.0f, coord.y+1.0f, coord.z,   TEXCOORDX(ts.back_x)+N,TEXCOORDY(ts.back_y)+N,  0, 0, 1,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}

void addLeftFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x, coord.y, coord.z+1.0f,   TEXCOORDX(ts.left_x),  TEXCOORDY(ts.left_y),  -1, 0, 0,
        coord.x, coord.y+1.0f, coord.z,   TEXCOORDX(ts.left_x)+N,TEXCOORDY(ts.left_y)+N,  -1, 0, 0,
        coord.x, coord.y, coord.z,   TEXCOORDX(ts.left_x)+N,TEXCOORDY(ts.left_y),  -1, 0, 0,
        coord.x, coord.y, coord.z+1.0f,   TEXCOORDX(ts.left_x),  TEXCOORDY(ts.left_y),  -1, 0, 0,
        coord.x, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.left_x),  TEXCOORDY(ts.left_y)+N,  -1, 0, 0,
        coord.x, coord.y+1.0f, coord.z,   TEXCOORDX(ts.left_x)+N,TEXCOORDY(ts.left_y)+N,  -1, 0, 0,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}


void addRightFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt) {

    TextureSides ts = getTextureSides(bt);

    GLfloat vert_uv_data[] = {
        coord.x+1.0f, coord.y, coord.z+1.0f,   TEXCOORDX(ts.right_x)+N,TEXCOORDY(ts.right_y),  1, 0, 0,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.right_x),  TEXCOORDY(ts.right_y), 1, 0, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.right_x)+N,TEXCOORDY(ts.right_y)+N, 1, 0, 0,
        coord.x+1.0f, coord.y, coord.z,   TEXCOORDX(ts.right_x),  TEXCOORDY(ts.right_y), 1, 0, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z,   TEXCOORDX(ts.right_x),  TEXCOORDY(ts.right_y)+N, 1, 0, 0,
        coord.x+1.0f, coord.y+1.0f, coord.z+1.0f,   TEXCOORDX(ts.right_x)+N,TEXCOORDY(ts.right_y)+N, 1, 0, 0,
    };
    buffer_data.insert(buffer_data.end(), vert_uv_data, vert_uv_data+48);
}