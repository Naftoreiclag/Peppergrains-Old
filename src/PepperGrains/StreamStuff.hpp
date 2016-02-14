#ifndef PGG_STREAMSTUFF_HPP
#define PGG_STREAMSTUFF_HPP

#include <string>
#include <fstream>
#include <stdint.h>

namespace pgg {

uint32_t readU32(std::ifstream& input);

bool readBool(std::ifstream& input);

uint16_t readU16(std::ifstream& input);

uint8_t readU8(std::ifstream& input);

float readF32(std::ifstream& input);

std::string readString(std::ifstream& input);

}

#endif // PGG_STREAMSTUFF_HPP
