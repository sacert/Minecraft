#include "Block.h"
#include "libraries/FastNoise.h"
#include <unordered_map>

#define CHUNK_SIZE 16 // x & z
#define CHUNK_HEIGHT 256 // y

struct Coordinates {
    int x;
    int y;
    int z;

    Coordinates (int xx, int yy, int zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    bool operator==(const Coordinates &other) const
    { return (x == other.x
                && y == other.y
                && z == other.z);
    }
};

namespace std {

    // include this to be able to hash Coordinates
    template <>
    struct hash<Coordinates>
    { 
        std::size_t operator()(const Coordinates& k) const
        {
        using std::size_t;
        using std::hash;

        return ((hash<int>()(k.x)
                ^ (hash<int>()(k.y) << 1)) >> 1)
                ^ (hash<int>()(k.z) << 1);
        }
    };
}

class Chunk {
    public:
        Chunk(int x, int z);
        void createChunk();
        void renderChunk();
        std::unordered_map<Coordinates, BlockType> getBlocks();
    private:
        int x;
        int z;
        std::unordered_map<Coordinates, BlockType> blocks;
        FastNoise perlinNoise;
};