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

#include <cmath>

namespace pgg {

SineWaveform::SineWaveform(float frequency)
: mFrequency(frequency) {
}

SineWaveform::~SineWaveform() {
}

void SineWaveform::mix(
    const Sound::Sample::Modifiers& modifiers, double callTime, 
    SoundIoChannelArea* channels, uint32_t channelCount, 
    uint32_t frameCount, uint32_t sampleRate) const
{
    float frameDuration = 1.f / sampleRate;
    float radSpeed = mFrequency * 6.2831853f;
    
    for(uint32_t channelIndex = 0; channelIndex < channelCount; ++ channelIndex) {
        SoundIoChannelArea& channel = channels[channelIndex];
        
        
        for(uint32_t frame = 0; frame < frameCount; ++ frame) {
            float sineWave = sinf((callTime + frame * frameDuration) * radSpeed);
            float& sample = *reinterpret_cast<float*>(channel.ptr + channel.step * frame);
            sample += sineWave;
        }
    }
}




} // namespace pgg

