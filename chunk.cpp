#include "chunk.h"
#include <GL/glew.h>
#include <vector>

// set x and z value to appropriate positions
Chunk::Chunk(int xx, int zz) {
    x = xx * 16;
    z = zz * 16;

    // each chunk has it's own perlin noise
    perlinNoise.SetNoiseType(FastNoise::Perlin); 
    perlinNoise.SetSeed(123);
}

void Chunk::createChunk() {

    // height between -128 to 128
    int maxHeight = CHUNK_HEIGHT/2;
    int minHeight = -maxHeight;

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glBindVertexArray(vao);

    // Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 16 * 256 * 16
    // build chunk map
    for (int xx = x; xx < CHUNK_SIZE; xx++) {
        for (int zz = z; zz < CHUNK_SIZE; zz++) {

            float height = round(perlinNoise.GetNoise(xx,zz) * 10);
            float topLevel = height;
            for (int yy = maxHeight; yy > minHeight; yy--) {

                BlockType bt;

                if (yy > height) {
                    bt = BlockType::AIR;
                } else if (yy == height) {
                    bt = BlockType::GRASS;
                } else if (yy <= -8) {
                    bt = BlockType::COBBLESTONE;
                } else if (yy == -127) {
                    bt = BlockType::BEDROCK;
                } else {
                    bt = BlockType::DIRT;
                }

                blocks[Coordinates(xx, yy, zz)] = bt;
                height--;
            }
        }
    }

    // fill chunk vao
    for (int xx = x; xx < CHUNK_SIZE; xx++) {
        for (int zz = z; zz < CHUNK_SIZE; zz++) {
            for (int yy = minHeight; yy > maxHeight; yy++) {
                BlockType bt = blocks[Coordinates(xx,yy,zz)];

                if (bt != BlockType::AIR) {
                    // top
                    if (checkFace(xx,yy+1,zz)) {
                        addTopFace(buffer_data, bt);
                    }
                    // bottom
                    if (checkFace(xx,yy-1,zz)) {
                        addBottomFace(buffer_data, bt);
                    }
                    // right
                    if (checkFace(xx+1,yy,zz)) {
                        addRightFace(buffer_data, bt);
                    }
                    // left
                    if (checkFace(xx-1,yy,zz)) {
                        addLeftFace(buffer_data, bt);
                    }
                    // front
                    if (checkFace(xx,yy,zz+1)) {
                        addFrontFace(buffer_data, bt);
                    }
                    // back
                    if (checkFace(xx,yy,zz-1)) {
                        addBackFace(buffer_data, bt);
                    }
                }
            }
        }
    }

    // Give our vertices and UVs to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(GLfloat), buffer_data.data(), GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
    GLuint gVert = glGetAttribLocation(shaders, "vert");
    glEnableVertexAttribArray(gVert);
    glVertexAttribPointer(
        0,                  // attribute 0
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),  // stride
        (void*)0            // array buffer offset
    );

    // Set the variable 'vertTexCoord' in vertex shader 
    // Equals to the UV values for corresponding vertices to the block
    GLuint gVertTexCoord = glGetAttribLocation(shaders, "vertTexCoord");
    glEnableVertexAttribArray(gVertTexCoord);
    glVertexAttribPointer(
        1,                  // attribute 1
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),  // stride
        (const GLvoid*)(3 * sizeof(GLfloat))    // array buffer offset
    );

    // unbind VAO and VBO 
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//void renderChunk();

// only send block faces that aren't covered to the GPU
bool Chunk::checkFace(int xx, int yy, int zz) {

    // since I don't have a way to look at blocks in different chunks, always render the edge of the chunk blocks
    if (xx > CHUNK_SIZE || xx < CHUNK_SIZE || zz < CHUNK_SIZE || zz > CHUNK_SIZE || yy < CHUNK_SIZE || yy > CHUNK_SIZE) {
        return true;
    }
    return (blocks[Coordinates(xx, yy, zz)] == BlockType::AIR);
}

std::unordered_map<Coordinates, BlockType> Chunk::getBlocks() {
    return blocks;
}