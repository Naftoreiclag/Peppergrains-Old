/*
   Copyright 2015-2017 James Fong

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

#ifndef PGG_ENTSIGNAL_HPP
#define PGG_ENTSIGNAL_HPP

namespace pgg {

// Note to self: Do not shorten to ESig

/*
 * Concrete classes are prefixed with "who" should be
 * sending the signal. For example, InputMoveSignal should
 * be initated directly by player input and PhysicsLocationUpdate
 * should be initiated by a system that works with physics.
 */
class ESignal {
public:
    enum Type {
        INPUT_MOVE, // Player input requests movement
        INPUT_INTERACT, // Player input interaction
        PHYSICS_LOCATION, // Physics changed location
        PHYSICS_ORIENTATION, // Physics changed orientation (i.e. rotation)
        ANIMATION // Play an animation
    };
public:
    ESignal();
    virtual ~ESignal();
    
    virtual Type getType() const = 0;

};

}

#endif // PGG_ENTSIGNAL_HPP
