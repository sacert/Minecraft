#include "chunk_manager.h"
#include <iostream>

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
    
    Coordinates aCoord(blockCoord.x/16, blockCoord.y/16, blockCoord.z/16);
    Chunk * chunk = &chunks.at(aCoord);
    chunk->addBlock(blockCoord, bt);
    chunk->updateChunk();
}

void ChunkManager::removeBlock(Coordinates blockCoord) {
    
    Coordinates aCoord(blockCoord.x/16, blockCoord.y/16, blockCoord.z/16);
    Chunk * chunk = &chunks.at(aCoord);
    chunk->removeBlock(blockCoord);
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
