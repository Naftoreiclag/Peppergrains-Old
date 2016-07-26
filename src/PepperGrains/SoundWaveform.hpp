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

#ifndef PGG_SOUND_WAVEFORM_HPP
#define PGG_SOUND_WAVEFORM_HPP

#include "ReferenceCounted.hpp"

#include "SoundSample.hpp"

namespace pgg {
namespace Sound {

/* The "raw" sound; no effects applied. Used by Samples during final mixing to
 * build the final buffer used by the Endpoint to send to the sound card.
 * 
 * This class is meant to be extended for sounds which are constructed in different
 * ways, such as a MIDI or voice synthesizer.
 * 
 * It is typical for a single waveform to be used by many samples.
 */
class Waveform {
public:
    Waveform();
    virtual ~Waveform();
    
    virtual void mix(
        const Sample::Modifiers& modifiers, double callTime, 
        SoundIoChannelArea* channels, uint32_t channelCount, 
        uint32_t frameCount, 
        uint32_t sampleRate) const = 0;
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_WAVEFORM_HPP