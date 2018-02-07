#include "block.h"
#include "libraries/FastNoise.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include <iostream>

#define CHUNK_SIZE 16 // x & z
#define CHUNK_HEIGHT 256 // y

class Chunk {
    public:
        Chunk(int x, int z, Camera *cam, BlockType ***block);
        ~Chunk();
        void createChunk();
        void updateBlock(Coordinates blockCoord);
        void renderChunk();
        BlockType getBlock(Coordinates blockCoord);
        int getX() const {return x;}
        int getZ() const {return z;}
        void addBlock(Coordinates blockCoord, BlockType bt);
        void removeBlock(Coordinates blockCoord);
    private:   
        int x;
        int z;
        double faces;
        GLuint vbo;
        GLuint vao;
        std::unordered_map<Coordinates, BlockType> blocks;
        FastNoise perlinNoise;
        std::vector<GLfloat> buffer_data;
        bool checkFace(int x, int y, int z);
        GLint shaders;
        Texture* texture;
        Camera* camera;
        BlockType*** blocksA;
};
