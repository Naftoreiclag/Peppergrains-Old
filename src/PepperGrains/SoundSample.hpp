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

#ifndef PGG_SOUND_SAMPLE_HPP
#define PGG_SOUND_SAMPLE_HPP

#include "SoundWaveform.hpp"

namespace pgg {
namespace Sound {

/* Combination of a waveform (the "raw" sound) and any effects
 * that need to be applied to it (e.g. change in speed). During final mixing,
 * the Endpoint builds a list of all samples that need to be output to the
 * sound card. Samples are not evaluated until this final mixing process.
 * 
 * Samples typically share a single Waveform with other Samples.
 */
class Sample {
public:
    struct Modifiers {
        float speed;
        float time;
    };
public:
    Sample();
    ~Sample();

    Waveform* mWaveform;
    Modifiers mModifiers;
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_SAMPLE_HPP
