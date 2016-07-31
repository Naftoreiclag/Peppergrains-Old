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
class Endpoint;

/* Gathers sounds; analogous to an ear or microphone. During final mixing, the
 * Receiever queries the Context to construct the list of samples to be used by
 * the Endpoint.
 */
class Receiver : public ReferenceCounted {
public:
    Vec3 mLocation;
    Context* mContext;
    Endpoint* mEndpoint;
    
    Context* mRequestedContext;
    Endpoint* mRequestedEndpoint;
    
public:
    Receiver(Context* context = nullptr, Endpoint* endpoint = nullptr);
    ~Receiver();
    
    std::vector<Sample*> mSamples;
    
    // Note: due to multithreading, this change is delayed until after the next endpoint update
    void setContext(Context* context);
    void unsetContext(); // Equivalent to setContext(nullptr);
    
    // Note: due to multithreading, this change is delayed until after the next endpoint update
    void setEndpoint(Endpoint* endpoint);
    void unsetEndpoint(); // Equivalent to setEndpoint(nullptr);
    
    void updateCalc(double time, Endpoint* endpnt);
    
    void load();
    void unload();
    
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_RECEIVER_HPP
