#include "block.h"
#include "libraries/FastNoise.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

#define CHUNK_SIZE 16 // x & z
#define CHUNK_HEIGHT 256 // y

class Chunk {
    public:
        Chunk(int x, int z, Camera *cam);
        ~Chunk();
        void createChunk();
        void renderChunk();
        std::unordered_map<Coordinates, BlockType> getBlocks();
    private:
        int x;
        int z;
        int faces;
        GLuint vbo;
        GLuint vao;
        std::unordered_map<Coordinates, BlockType> blocks;
        FastNoise perlinNoise;
        std::vector<GLfloat> buffer_data;
        bool checkFace(int x, int y, int z);
        GLint shaders;
        Texture* texture;
        Camera* camera;
};