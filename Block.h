#include <vector>
#include <GL/glew.h>

#define N -0.0625   // normalized size of each block in texture file - negative for opengl
#define TEXCOORDY(Y) (1-(0.0625 * Y))    // corrected way to find y coordinates of texture file
#define TEXCOORDX(X) (0.0625+(0.0625 * X))    // corrected way to find x coordinates of texture file

enum BlockType {
    AIR,
    GRASS,
    DIRT,
    COBBLESTONE,
    BEDROCK,
};

struct TextureSides {
    int bottom_x;
    int bottom_y;
    int top_x;
    int top_y;
    int right_x;
    int right_y;
    int left_x;
    int left_y;
    int front_x;
    int front_y;
    int back_x;
    int back_y;
};

TextureSides sameSides(int x, int y);
TextureSides differentSides(int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y);
TextureSides getTextureSides(BlockType bt);

void addBottomFace(std::vector<GLfloat> &buffer_data, BlockType bt);
void addTopFace(std::vector<GLfloat> &buffer_data, BlockType bt);
void addRightFace(std::vector<GLfloat> &buffer_data, BlockType bt);
void addLeftFace(std::vector<GLfloat> &buffer_data, BlockType bt);
void addFrontFace(std::vector<GLfloat> &buffer_data, BlockType bt);
void addBackFace(std::vector<GLfloat> &buffer_data, BlockType bt);

//GLint shaders = LoadShaders( "shaders/block_vertex.glsl", "shaders/block_fragment.glsl" );


