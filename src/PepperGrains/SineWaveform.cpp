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

#include "SineWaveform.hpp"

#include <iostream>
#include <cmath>

namespace pgg {

SineWaveform::SineWaveform(float frequency)
: mFrequency(frequency) {
}

SineWaveform::~SineWaveform() {
}

void SineWaveform::mix(
    SoundIoChannelArea* channels, uint32_t channelCount, 
    uint32_t frameCount, 
    double start,
    double end) const
{
    double frameDuration = (end - start) / frameCount;
    float radSpeed = mFrequency * 6.2831853f;
    
    for(uint32_t channelIndex = 0; channelIndex < channelCount; ++ channelIndex) {
        SoundIoChannelArea& channel = channels[channelIndex];
        
        for(uint32_t frame = 0; frame < frameCount; ++ frame) {
            double time = start + frameDuration * frame;
            float sineWave = sinf(time * radSpeed);
            float& sample = *reinterpret_cast<float*>(channel.ptr + channel.step * frame);
            sample += sineWave;
        }
    }
}
void SineWaveform::load() {
    
}
void SineWaveform::unload() {
    //delete this;
}




} // namespace pgg

