#include "camera.h"
#include "chunk.h"
#include <unordered_map>

class ChunkManager {
    public:
        ChunkManager(Camera *camera);
        void initChunks();                          // depending on player location + rendering distance, load chunks around player
        void updateChunks();                        // as the player moves, add/remove chunks to fit rendering distance
        void addBlock(Coordinates blockCoord);      // puts block in corresponding chunk (x, y, z are true)
        void removeBlock(Coordinates blockCoord);   // delete block in corresponding chunk
        Chunk findChunk(Coordinates blockCoord);    // used by add/removeBlock
        void addChunk(Coordinates chunkCoord);
        void removeChunk(Coordinates chunkCoord);
        void renderChunks();                        // when rendering chunks, pass camera            
    private:
        // will need player position to add and remove chunks for the render distance
        float player_x;
        float player_z;
        std::unordered_map<Coordinates, Chunk> chunks;  // x z = val / y = 0 
        Camera* camera;
}