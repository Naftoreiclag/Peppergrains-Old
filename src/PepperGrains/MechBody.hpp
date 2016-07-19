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

#ifndef PGG_MECHBODY_HPP
#define PGG_MECHBODY_HPP

#include <vector>
#include <stdint.h>

namespace pgg {

class MechBody {
private:
    class Plate {
        
        Plate();
        virtual ~Plate();
        
        int32_t mLocX;
        int32_t mLocY;
        int32_t mLocZ;
        
        int32_t mScaleX;
        int32_t mScaleY;
        int32_t mScaleZ;
    };
    
public:
    MechBody();
    ~MechBody();
    
    std::vector<Plate*> mPlates;
};

}

#endif // PGG_MECHBODY_HPP
