#ifndef __CHUNK_HPP__
#define __CHUNK_HPP__

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace engine {

enum BlockType : uint8_t {
    AIR     = 0,
    DIRT    = 1,
    GRASS   = 2,
    STONE   = 3,
    NUMBER_OF_TYPES
};

class Chunk {
public:
    static constexpr size_t LENGTH = 32;
    static constexpr size_t SIZE = LENGTH * LENGTH * LENGTH;

    Chunk() : blocks{SIZE, AIR} {}

    // Helper to get index in the 1D array
    size_t getIndex(uint8_t x, uint8_t y, uint8_t z) const {
        return x + (y * LENGTH) + (z * LENGTH * LENGTH);
    }

    BlockType getBlock(uint8_t x, uint8_t y, uint8_t z) const {
        if (x < 0 || x >= LENGTH || y < 0 || y >= LENGTH || z < 0 || z >= LENGTH) {
            throw std::runtime_error("Access violation: getBlock(int,int,int) index out of bounds!");
        }
        return blocks[getIndex(x, y, z)];
    }

    void setBlock(int x, int y, int z, BlockType type) {
        if (x < 0 || x >= LENGTH || y < 0 || y >= LENGTH || z < 0 || z >= LENGTH) {
            throw std::runtime_error("Access violation: setBlock(int,int,int,BlockType) index out of bounds!");
        }
        blocks[getIndex(x, y, z)] = type;
    }
private:
    std::vector<BlockType> blocks;
};

} // namespace engine

#endif