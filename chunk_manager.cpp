#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(Camera *cam) {
    camera = cam;
}

// add chunk into the map from save file if already created before otherwise create new chunk
void ChunkManager::addChunk(Coordinates chunkCoord) {

     //  allocate memory for blocks - is there an easier way to do this?
    BlockType*** blocks = new BlockType**[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        blocks[i] = new BlockType*[CHUNK_HEIGHT];
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            blocks[i][j] = new BlockType[CHUNK_SIZE];
        }
    }

    Chunk chunk(chunkCoord.x, chunkCoord.z, camera, blocks);

    // only create chunk if isn't in the 'save file' - which isn't implemented yet
    chunk.createChunk();

    //else 
    //chunk.retrieveChunk();

    chunks.insert(std::make_pair(chunkCoord, chunk));

}

void ChunkManager::addBlock(Coordinates blockCoord, BlockType bt) {
    
    int x = blockCoord.x;
    int z = blockCoord.z;
    while(1) {
        if (x  % 16 == 0) {
            break;
        }
        x--;
    }

    while(1) {
        if (z  % 16 == 0) {
            break;
        }
        z--;
    }
    Coordinates aCoord(x/16, 0, z/16);

    auto it=chunks.find(aCoord);
    if (it==chunks.end()) 
        return;
    it->second.addBlock(blockCoord, bt);
    it->second.updateChunk(getNeighbours(aCoord));
}

void ChunkManager::removeBlock(Coordinates blockCoord) {
    int x = blockCoord.x;
    int z = blockCoord.z;
    while(1) {
        if (x  % 16 == 0) {
            break;
        }
        x--;
    }

    while(1) {
        if (z  % 16 == 0) {
            break;
        }
        z--;
    }
    Coordinates aCoord(x/16, 0, z/16);
    auto it=chunks.find(aCoord);
    if (it==chunks.end()) 
        return;
    it->second.removeBlock(blockCoord);
    it->second.updateChunk(getNeighbours(aCoord));
}

void ChunkManager::updateChunk(Coordinates chunkCoord) {
    Chunk* chunk = &chunks.at(chunkCoord);
    chunk->updateChunk(getNeighbours(chunkCoord));
}

// get the neighbouring chunks and return in an array
Chunk* ChunkManager::getNeighbours(Coordinates chunkCoord) {

    // 0 = right
    // 1 = left
    // 2 = front
    // 3 = back

    Chunk* neighbours = new Chunk[4];
    
    std::unordered_map<Coordinates, Chunk>::const_iterator got;
    
    Coordinates coordRight(chunkCoord.x+1, chunkCoord.y, chunkCoord.z);
    got = chunks.find(coordRight);

    if ( got != chunks.end() ) 
        neighbours[0] = got->second;
    
    Coordinates coordLeft(chunkCoord.x-1, chunkCoord.y, chunkCoord.z);
    got = chunks.find(coordLeft);

    if ( got != chunks.end() ) 
        neighbours[1] = got->second;

    Coordinates coordFront(chunkCoord.x, chunkCoord.y, chunkCoord.z+1);
    got = chunks.find(coordFront);

    if ( got != chunks.end() ) 
        neighbours[2] = got->second;
    
    Coordinates coordBehind(chunkCoord.x, chunkCoord.y, chunkCoord.z-1);
    got = chunks.find(coordBehind);

    if ( got != chunks.end() ) 
        neighbours[3] = got->second;

    return neighbours;
}

BlockType ChunkManager::getBlock(Coordinates blockCoord) {

    int x = blockCoord.x;
    int z = blockCoord.z;
    while(1) {
        if (x  % 16 == 0) {
            break;
        }
        x--;
    }

    while(1) {
        if (z  % 16 == 0) {
            break;
        }
        z--;
    }
    int xx = x /16;
    int zz = z /16;
    Coordinates aCoord(xx, 0, zz); 

    //std::cout << aCoord.x << " " << aCoord.y << " " <<aCoord.z << std::endl;

    // need to check if chunk is even there -- impossible once chunks are generated around user
    auto it=chunks.find(aCoord);
    if (it==chunks.end()) 
        return BlockType::AIR;
    return it->second.getBlock(blockCoord);
}

Chunk ChunkManager::findChunk(Coordinates chunkCoord) {

    std::unordered_map<Coordinates, Chunk>::const_iterator got = chunks.find (chunkCoord);
    return got->second;
}

void ChunkManager::removeChunk(Coordinates chunkCoord) {
    chunks.erase(chunkCoord);
}

void ChunkManager::renderChunks() {
    Chunk::chunksNum = 0;
    for ( auto it = chunks.begin(); it != chunks.end(); ++it ) {
       it->second.renderChunk();
    }

    std::cout << Chunk::chunksNum << std::endl;
}
