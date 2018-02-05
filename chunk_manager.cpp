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

    std::unordered_map<Coordinates, Chunk> chunks;
    chunks.insert(std::make_pair(chunkCoord, chunk));
}

void ChunkManager::removeChunk(Coordinates chunkCoord) {
    chunks.erase(chunkCoord);
}

void ChunkManager::renderChunks() {
    for ( auto it = chunks.begin(); it != chunks.end(); ++it )
       it->second.renderChunk();
}