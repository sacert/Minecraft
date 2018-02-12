#include "chunk_manager.h"
#include <iostream>
#include <cmath>

ChunkManager::ChunkManager(Camera *cam) {
    camera = cam;

    // keep track of the chunk the player was previously in - for prodcedural map generation
    oldPos.x = 0;
    oldPos.z = 0;
}

// add chunk into the map from save file if already created before otherwise create new chunk
void ChunkManager::addChunk(Coordinates chunkCoord) {

    Chunk chunk = Chunk(chunkCoord.x, chunkCoord.z, camera);

    // only create chunk if isn't in the 'save file' - which isn't implemented yet
    chunk.createChunk();

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
    Chunk* neighbours = it->second.getNeighbours();
    for (int i = 0; i < 4; i++) {
        Coordinates aCoord(neighbours[i].getX()/16, 0, neighbours[i].getZ()/16);
        auto itt=chunks.find(aCoord);
        if (itt==chunks.end()) 
            continue;
        if (!(itt->second.isEmpty())) {
            itt->second.updateChunk(getNeighbours(Coordinates(itt->second.getX()/16, 0, itt->second.getZ()/16)));
        }
    }
}

void ChunkManager::removeBlock(Coordinates blockCoord) {

    // get the closest floor values to a multiple of 16
    int x = getChunkPos(blockCoord.x);
    int z = getChunkPos(blockCoord.z);
    Coordinates aCoord(x, 0, z);

    auto it=chunks.find(aCoord);
    if (it==chunks.end()) 
        return;
    it->second.removeBlock(blockCoord);
    it->second.updateChunk(getNeighbours(aCoord));

    // update the neighbours so they know edge chunks are no longer visible
    Chunk* neighbours = it->second.getNeighbours();
    for (int i = 0; i < 4; i++) {
        Coordinates aCoord(neighbours[i].getX()/16, 0, neighbours[i].getZ()/16);
        auto itt=chunks.find(aCoord);
        if (itt==chunks.end()) 
            continue;
        if (!(itt->second.isEmpty())) {
            itt->second.updateChunk(getNeighbours(Coordinates(itt->second.getX()/16, 0, itt->second.getZ()/16)));
        }
    }
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

    int x = getChunkPos(blockCoord.x);
    int z = getChunkPos(blockCoord.z);
    Coordinates aCoord(x, 0, z); 

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

bool ChunkManager::emptyChunk(Coordinates chunkCoord) {
    auto it=chunks.find(chunkCoord);
    if (it==chunks.end()) 
        return true;
    return false;
}

void ChunkManager::removeChunk(Coordinates chunkCoord) {
    chunks.erase(chunkCoord);
}

void ChunkManager::renderChunks(Coordinates selected) {
    for ( auto it = chunks.begin(); it != chunks.end(); ++it ) {
       it->second.renderChunk(selected);
    }
}

void ChunkManager::proceduralMapUpdate(glm::vec3 currPos) {

    currPos.x = getChunkPos(currPos.x);
    currPos.z = getChunkPos(currPos.z);

    if (oldPos.x == currPos.x && oldPos.z == currPos.z) {
        return;
    }

    std::vector<Coordinates> list;

    // for all chunks, if any chunks are not in the range of the newChunk's radius, delete them
    for ( auto it = chunks.begin(); it != chunks.end(); ++it ) {
        if (it->second.getX()/16 > (1 + currPos.x) || it->second.getZ()/16 < (currPos.z-1)) {
                    std::cout << it->second.getX()/16 << " " << it->second.getZ()/16 << std::endl;

                list.push_back(Coordinates(it->second.getX()/16, 0, it->second.getZ()/16));
        }
    }

    for (int i = 0; i < list.size(); i++) {
        removeChunk(list.at(i));
    }

    std::cout << "ADDING" << std::endl;
    for (int i = currPos.x-2; i < currPos.x+2; i++) {
        for (int j = currPos.z-2; j < currPos.z+2; j++) {
            if (emptyChunk(Coordinates(i, 0, j))) {
                addChunk(Coordinates(i,0,j));
            } 
        }
    }

    oldPos.x = currPos.x;
    oldPos.z = currPos.z;
}


int ChunkManager::getChunkPos(int val) {
    while(1) {
        if (val  % 16 == 0) 
            break;
        val--;
    }
    return val/16;
}
