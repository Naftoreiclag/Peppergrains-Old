/*
   Copyright 2016 James Fong

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef PGG_SPHERICALHARMONICS_HPP
#define PGG_SPHERICALHARMONICS_HPP

#include <stdint.h>

// Spharm: SPherical HARMonics
// low order L = 3 (16 coefficients)

namespace pgg {

class Spharm {
public:
    Spharm();
    ~Spharm();
    
    float coeff[16];
    
    static float yml(uint8_t l, uint8_t m, float phi, float theta);
    float eval(float phi, float theta) const;

};

}

#endif // PGG_SPHERICALHARMONICS_HPP
