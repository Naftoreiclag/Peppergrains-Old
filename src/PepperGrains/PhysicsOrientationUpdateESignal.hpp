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

#ifndef PGG_PHYSICSORIENTATIONUPDATEESYS_HPP
#define PGG_PHYSICSORIENTATIONUPDATEESYS_HPP

#include "NRES.hpp"

#include "EntitySignal.hpp"
#include "Quate.hpp"

namespace pgg {

class PhysicsOrientationUpdateESignal : public ESignal {
public:
    PhysicsOrientationUpdateESignal(const Quate& quaternion);
    ~PhysicsOrientationUpdateESignal();
    
    const Quate mOrientation;
    
    ESignal::Type getType() const;

};

}

#endif // PGG_PHYSICSORIENTATIONUPDATEESYS_HPP
