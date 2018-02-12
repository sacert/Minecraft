#include "camera.h"
#include "chunk.h"

class ChunkManager {
    public:
        ChunkManager(Camera *camera);
        void initChunks();                          // depending on player location + rendering distance, load chunks around player
        void updateChunks();                        // as the player moves, add/remove chunks to fit rendering distance + dirty chunk
        void addBlock(Coordinates blockCoord, BlockType bt);      // puts block in corresponding chunk (x, y, z are true)
        void removeBlock(Coordinates blockCoord);   // delete block in corresponding chunk
        BlockType getBlock(Coordinates blockCoord);
        Chunk findChunk(Coordinates chunkCoord);    // used by add/removeBlock
        void addChunk(Coordinates chunkCoord);
        void removeChunk(Coordinates chunkCoord);
        void renderChunks(Coordinates selected);                        // when rendering chunks, pass camera  
        Chunk* getNeighbours(Coordinates chunkCoord);
        void updateChunk(Coordinates chunkCoord);
        void proceduralMapUpdate(glm::vec3 currPos);
        bool emptyChunk(Coordinates chunkCoord);
    private:
        int getChunkPos(int val);
        glm::vec3 oldPos;
        std::unordered_map<Coordinates, Chunk> chunks;  // x z = val / y = 0 
        Camera *camera;
};