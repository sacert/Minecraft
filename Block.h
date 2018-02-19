#include <vector>
#include <GL/glew.h>
#include <unordered_map>

#define N -0.0625   // normalized size of each block in texture file - negative for opengl
#define TEXCOORDY(Y) (1-(0.0625 * Y))    // corrected way to find y coordinates of texture file
#define TEXCOORDX(X) (0.0625+(0.0625 * X))    // corrected way to find x coordinates of texture file

enum BlockType {
    AIR,
    GRASS,
    DIRT,
    COBBLESTONE,
    BEDROCK,
    SAND,
    LEAVES,
    WOOD,
    GRASS_PLANT,
    FLOWER_RED_PLANT,
    FLOWER_YELLOW_PLANT,
    MUSHROOM_RED,
    MUSHROOM_PINK,
    FERN_1,
    FERN_2,
    EMPTY,
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

struct Coordinates {
    int x;
    int y;
    int z;

    Coordinates (int xx, int yy, int zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    bool operator==(const Coordinates &other) const
    { return (x == other.x
                && y == other.y
                && z == other.z);
    }
};

namespace std {

    // include this to be able to hash Coordinates
    template <>
    struct hash<Coordinates>
    { 
        std::size_t operator()(const Coordinates& k) const
        {
        using std::size_t;
        using std::hash;

        return ((hash<int>()(k.x)
                ^ (hash<int>()(k.y) << 1)) >> 1)
                ^ (hash<int>()(k.z) << 1);
        }
    };
}

TextureSides sameSides(int x, int y);
TextureSides differentSides(int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y);
TextureSides getTextureSides(BlockType bt);

// for plants
void addCrossFace_1(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addCrossFace_2(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);

// for blocks
void addBottomFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addTopFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addRightFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addLeftFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addFrontFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);
void addBackFace(std::vector<GLfloat> &buffer_data, Coordinates coord, BlockType bt);


