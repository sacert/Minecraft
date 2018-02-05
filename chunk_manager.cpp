#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(Camera *cam) {
    camera = cam;
}

// add chunk into the map from save file if already created before otherwise create new chunk
void ChunkManager::addChunk(Coordinates chunkCoord) {

    Chunk chunk(chunkCoord.x, chunkCoord.z, camera);

    // only create chunk if isn't in the 'save file' - which isn't implemented yet
    chunk.createChunk();

    //else 
    //chunk.retrieveChunk();

    chunks.insert(std::make_pair(chunkCoord, chunk));

}

void ChunkManager::addBlock(Coordinates blockCoord, BlockType bt) {
    
    Coordinates aCoord(floor((blockCoord.x-16)/16), 0, floor((blockCoord.z-16)/16));
    Chunk * chunk = &chunks.at(aCoord);
    chunk->addBlock(blockCoord, bt);
    chunk->updateBlock(blockCoord);
}

void ChunkManager::removeBlock(Coordinates blockCoord) {
    
    Coordinates aCoord(floor((blockCoord.x-16)/16), 0, floor((blockCoord.z-16)/16));
    Chunk * chunk = &chunks.at(aCoord);
    chunk->removeBlock(blockCoord);
    chunk->updateBlock(blockCoord);
}

BlockType ChunkManager::getBlock(Coordinates blockCoord) {
    Coordinates aCoord(floor((blockCoord.x-16)/16), 0, floor((blockCoord.z-16)/16));

    auto it=chunks.find(aCoord);
    if (it==chunks.end()) 
        return BlockType::AIR;
    return it->second.getBlock(blockCoord);

    // Chunk *chunk = &chunks.at(aCoord);
    // return chunk->getBlock(blockCoord);
}

Chunk ChunkManager::findChunk(Coordinates chunkCoord) {

    std::unordered_map<Coordinates, Chunk>::const_iterator got = chunks.find (chunkCoord);
    return got->second;
}

void ChunkManager::removeChunk(Coordinates chunkCoord) {
    chunks.erase(chunkCoord);
}

void ChunkManager::renderChunks() {
    for ( auto it = chunks.begin(); it != chunks.end(); ++it ) {
       it->second.renderChunk();
    }
}
