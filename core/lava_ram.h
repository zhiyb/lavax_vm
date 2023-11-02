#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <stdexcept>
#define RAM_TODO()  throw std::runtime_error("RAM_TODO: " + std::to_string(__LINE__))

class LavaRam
{
public:
    LavaRam() {}

    void init(uint32_t rambits);

    std::vector<uint8_t> &data() {return ram;}


    void push(uint32_t v)
    {
        ram.at(stack + 0) = (uint8_t)(v >>  0);
        ram.at(stack + 1) = (uint8_t)(v >>  8);
        ram.at(stack + 2) = (uint8_t)(v >> 16);
        ram.at(stack + 3) = (uint8_t)(v >> 24);
        stack += 4;
    }

    uint32_t pop()
    {
        uint32_t v = 0;
        stack -= 4;
        v |= ram.at(stack + 0) <<  0;
        v |= ram.at(stack + 1) <<  8;
        v |= ram.at(stack + 2) << 16;
        v |= ram.at(stack + 3) << 24;
        return v;
    }

    uint32_t pushString(const std::vector<uint8_t> &str);


    uint8_t readU8(uint32_t a) {return ram[a];}
    void writeU8(uint32_t a, uint8_t v) {ram[a] = v;}

    int16_t readI16(uint32_t a)
    {
        return (int16_t)(ram[a] | (ram[a + 1] << 8));
    }
    void writeI16(uint32_t a, int16_t v)
    {
        ram[a + 0] = (uint32_t)v >> 0;
        ram[a + 1] = (uint32_t)v >> 8;
    }

    uint32_t readU32(uint32_t a)
    {
        return (int32_t)(ram[a] | (ram[a + 1] << 8) | (ram[a + 2] << 16) | (ram[a + 3] << 24));
    }
    int32_t readI32(uint32_t a) {return readU32(a);}
    void writeU32(uint32_t a, uint32_t v)
    {
        ram[a + 0] = (uint32_t)v >>  0;
        ram[a + 1] = (uint32_t)v >>  8;
        ram[a + 2] = (uint32_t)v >> 16;
        ram[a + 3] = (uint32_t)v >> 24;
    }
    void writeI32(uint32_t a, int32_t v) {writeU32(a, v);}


    uint32_t getStack() {return stack;}
    void setStack(uint32_t a) {stack = a;}
    uint32_t getLocalStack() {return stack_local;}
    void setLocalStack(uint32_t a) {stack_local = a;}
    uint32_t getLocalStackBp() {return stack_local_bp;}
    void setLocalStackBp(uint32_t a) {stack_local_bp = a;}

private:
    std::vector<uint8_t> ram;
    uint32_t stack, stack_string;
    uint32_t stack_local, stack_local_bp;
};
