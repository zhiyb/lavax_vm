#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include <stdexcept>
#define RAM_TODO()  throw std::runtime_error("RAM_TODO: " + std::to_string(__LINE__))

class LavaRam
{
public:
    LavaRam() {}

    void init(uint32_t rambits);

    std::vector<uint8_t> &data() {return ram;}

    uint32_t getAddrMask() {return ram_mask;}

    uint32_t getStack() {return stack;}
    void setStack(uint32_t a) {stack = a;}
    uint32_t getLocalStack() {return stack_local;}
    void setLocalStack(uint32_t a) {stack_local = a;}
    uint32_t getLocalStackBp() {return stack_local_bp;}
    void setLocalStackBp(uint32_t a) {stack_local_bp = a;}


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

    void pushInst(uint32_t a)
    {
        ram.at(stack_local + 0) = a >>  0;
        ram.at(stack_local + 1) = a >>  8;
        ram.at(stack_local + 2) = a >> 16;
    }

    uint32_t pushString(const std::vector<uint8_t> &dstr);


    uint8_t readU8(uint32_t a)
    {
        a &= ram_mask;
        return ram[a];
    }
    void writeU8(uint32_t a, uint8_t v)
    {
        a &= ram_mask;
        ram[a] = v;
    }

    int16_t readI16(uint32_t a)
    {
        a &= ram_mask;
        return (int16_t)(ram[a] | (ram[a + 1] << 8));
    }
    void writeI16(uint32_t a, int16_t v)
    {
        a &= ram_mask;
        ram[a + 0] = (uint32_t)v >> 0;
        ram[a + 1] = (uint32_t)v >> 8;
    }

    uint32_t readU32(uint32_t a)
    {
        a &= ram_mask;
        return (int32_t)(ram[a] | (ram[a + 1] << 8) | (ram[a + 2] << 16) | (ram[a + 3] << 24));
    }
    int32_t readI32(uint32_t a) {return readU32(a);}
    void writeU32(uint32_t a, uint32_t v)
    {
        a &= ram_mask;
        ram[a + 0] = (uint32_t)v >>  0;
        ram[a + 1] = (uint32_t)v >>  8;
        ram[a + 2] = (uint32_t)v >> 16;
        ram[a + 3] = (uint32_t)v >> 24;
    }
    void writeI32(uint32_t a, int32_t v) {writeU32(a, v);}

    uint32_t getAddrVariant(uint32_t a, uint8_t type)
    {
        a &= ram_mask;
        if (ram_bits <= 16)
            a |= type << 16;
        else
            a |= type << 24;
        return a;
    }
    uint32_t readVariant(uint32_t a)
    {
        uint8_t type;
        if (ram_bits <= 16)
            type = a >> 16;
        else
            type = a >> 24;

        if (type & 0x80)
            a += stack_local_bp;

        type &= 0x7f;
        a &= ram_mask;
        if (type == 1)
            return readU8(a);
        else if (type == 2)
            return readI16(a);
        else
            return readI32(a);
    }
    void writeVariant(uint32_t a, uint32_t v)
    {
        uint8_t type;
        if (ram_bits <= 16)
            type = a >> 16;
        else
            type = a >> 24;

        if (type & 0x80)
            a += stack_local_bp;

        type &= 0x7f;
        a &= ram_mask;
        if (type == 1)
            writeU8(a, v);
        else if (type == 2)
            writeI16(a, v);
        else
            writeI32(a, v);
    }

    std::vector<uint8_t> readData(uint32_t a, uint32_t size)
    {
        a &= ram_mask;
        std::vector<uint8_t> data;
        data.resize(size);
        std::copy(ram.cbegin() + a, ram.cbegin() + a + size, data.begin());
        return data;
    }
    void writeData(uint32_t a, const std::vector<uint8_t> &data)
    {
        a &= ram_mask;
        std::copy(data.cbegin(), data.cend(), ram.begin() + a);
    }

    void writeStringData(uint32_t a, const std::vector<uint8_t> &str)
    {
        a &= ram_mask;
        std::copy(str.cbegin(), str.cend(), ram.begin() + a);
    }
    std::vector<uint8_t> readStringData(uint32_t a)
    {
        std::vector<uint8_t> str;
        uint8_t v;
        do {
            v = ram[a++];
            str.push_back(v);
        } while (v != '\0');
        return str;
    }
    uint32_t strlen(uint32_t a)
    {
        uint32_t len = 0;
        for (; ram[a + len] != '\0'; len++);
        return len;
    }


private:
    std::string to_string(const std::vector<uint8_t> &data)
    {
        return std::string(reinterpret_cast<const char *>(data.data()), data.size() - 1);
    }

    std::vector<uint8_t> ram;
    uint32_t ram_bits, ram_mask;
    uint32_t stack, stack_string;
    uint32_t stack_local, stack_local_bp;

    // Stored pushed string constants
    std::unordered_map<std::string, uint32_t> str_map;
};
