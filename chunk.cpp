#include "chunk.h"

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

    // 16 * 256 * 16
    // build chunk map
    for (int xx = x; xx < CHUNK_SIZE; xx++) {
        for (int zz = z; zz < CHUNK_SIZE; zz++) {

            float height = round(perlinNoise.GetNoise(xx,zz) * 10);
            float topLevel = height;
            for (int yy = maxHeight; yy > minHeight; yy--) {

                BlockType bt;
                if (yy == height) {
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
}
void renderChunk();

std::unordered_map<Coordinates, BlockType> Chunk::getBlocks() {
    return blocks;
}