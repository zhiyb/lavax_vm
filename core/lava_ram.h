#pragma once

#include <cstdint>
#include <vector>
#include <string>

class LavaRam
{
public:
    LavaRam() {}

    void init(uint32_t rambits);

    void push(uint32_t v) {}
    uint32_t pushString(const std::string &str) {return stack_string;}

    uint32_t pop() {return 0;}
    uint8_t getU8(uint32_t a) {return 0;}
    int16_t getI16(uint32_t a) {return 0;}
    int32_t getI32(uint32_t a) {return 0;}

private:
    std::vector<uint8_t> ram;
    uint32_t stack, stack_string;
};
