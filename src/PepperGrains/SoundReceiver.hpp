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

#ifndef PGG_SOUND_RECEIVER_HPP
#define PGG_SOUND_RECEIVER_HPP

#include <vector>

#include "ReferenceCounted.hpp"
#include "SoundSample.hpp"
#include "Vec3.hpp"

namespace pgg {
namespace Sound {

class Context;

/* Gathers sounds; analogous to an ear or microphone. During final mixing, the
 * Receiever queries the Context to construct the list of samples to be used by
 * the Endpoint.
 */
class Receiver : public ReferenceCounted {
public:
    Receiver();
    ~Receiver();

    Vec3 mLocation;
    
    Context* mContext;
    
    void evaluate(std::vector<Sample*>& sampleList, const double& calltime);
    
    void load();
    void unload();
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_RECEIVER_HPP
