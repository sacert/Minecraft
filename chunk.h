#include "block.h"
#include "libraries/FastNoise.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "frustum.h"
#include <iostream>


#define CHUNK_SIZE 16 // x & z
#define CHUNK_HEIGHT 256 // y

class Chunk {
    public:
        Chunk();
        Chunk(int x, int z, Camera *cam, BlockType ***block);
        ~Chunk();
        void createChunk();
        void updateChunk(Chunk* neighbours);
        void updateChunk();
        void renderChunk(Coordinates selected);
        BlockType getBlock(Coordinates blockCoord);
        BlockType getBlockNormalized(Coordinates blockCoord);
        void addBlock(Coordinates blockCoord, BlockType bt);
        void removeBlock(Coordinates blockCoord);
        bool checkFace(int x, int y, int z);
        bool checkNeighbour(int x, int y, int z);
        int getX() {return chunk_x;}
        int getZ() {return chunk_z;}
        int isEmpty() {return empty;}
        Chunk * getNeighbours();
        static int chunksNum;
    private:   
        int chunk_x;
        int chunk_z;
        double faces;
        GLuint vbo;
        GLuint vao;
        std::unordered_map<Coordinates, BlockType> l;
        FastNoise perlinNoise;
        std::vector<GLfloat> buffer_data;
        GLint shaders;
        Texture* texture;
        Camera* camera;
        BlockType*** blocks;
        Frustum frustum;
        Chunk* neighbours;
        bool empty;
};
