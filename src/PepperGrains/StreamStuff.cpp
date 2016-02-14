#include "StreamStuff.hpp"

namespace pgg {

uint32_t readU32(std::ifstream& input) {
    uint32_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool readBool(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value != 0;
}

uint16_t readU16(std::ifstream& input) {
    uint16_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

uint8_t readU8(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value;
}

float readF32(std::ifstream& input) {
    float value;
    input.read((char*) &value, sizeof value);
    return value;
}

}
