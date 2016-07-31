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

#include "SoundEndpoint.hpp"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "PepperGrains.hpp"
#include "SoundWaveform.hpp"

namespace pgg {
namespace Sound {

Endpoint::Endpoint()
: mDevice(nullptr)
, mStream(nullptr) {
}

Endpoint::~Endpoint() {
    if(mStream) soundio_outstream_destroy(mStream);
    if(mDevice) soundio_device_unref(mDevice);
}

void Endpoint::setDevice(SoundIoDevice* device) {
    if(mDevice) {
        soundio_device_unref(mDevice);
        
        if(mStream) soundio_outstream_destroy(mStream);
    }
    
    mRuntime = PepperGrains::getSingleton()->getRunningTimeSeconds();
    
    mDevice = device;
    soundio_device_ref(mDevice);
    
    mStream = soundio_outstream_create(mDevice);
    
    if(!mStream) {
        std::cerr << "Ran out of memory while trying to create sound output stream!" << std::endl;
        return;
    }
    
    mStream->format = SoundIoFormatFloat32NE;
    mStream->write_callback = endpointSoundIoWriteCallback;
    mStream->userdata = this;
    //mStream->sample_rate = soundio_device_nearest_sample_rate(mDevice, 1);
    
    int error = soundio_outstream_open(mStream);
    if(error) {
        std::cout << "Sample rate: " << mStream->sample_rate << std::endl;
        std::cerr << "Error while opening audio stream: " << soundio_strerror(error) << std::endl;
        return;
    }
    if(mStream->layout_error) {
        std::cerr << "Error while setting channel layout: " << soundio_strerror(mStream->layout_error) << std::endl;
        return;
    }
    error = soundio_outstream_start(mStream);
    if(error) {
        std::cerr << "Error while starting audio stream: " << soundio_strerror(error) << std::endl;
        return;
    }
}

void Endpoint::writeCallback(SoundIoOutStream* stream, uint32_t minFrames, uint32_t maxFrames) {
    //double callTime = PepperGrains::getSingleton()->getRunningTimeSeconds();
    // std::cout << callTime << std::endl;
    
    const SoundIoChannelLayout& layout = stream->layout;
    
    uint32_t framesRemaining = 1000;
    if(framesRemaining < minFrames) { framesRemaining = minFrames; }
    if(framesRemaining > maxFrames) { framesRemaining = maxFrames; }
    uint32_t sampleRate = stream->sample_rate;
    
    double frameDuration = 1.0 / sampleRate;
    
    SoundIoChannelArea* channels;
    uint32_t channelCount = layout.channel_count;
    
    int frameCount;
    while(framesRemaining > 0) {
        frameCount = framesRemaining;
        int error = soundio_outstream_begin_write(stream, &channels, &frameCount);
        if(error) {
            // !!!
        }
        
        if(!frameCount) {
            break;
        }
        
        // Initialize channel with zeros (Not sure this is needed in all cases)
        for(uint32_t channelIndex = 0; channelIndex < channelCount; ++ channelIndex) {
            SoundIoChannelArea& channel = channels[channelIndex];
            for(int frame = 0; frame < frameCount; ++ frame) {
                float& sample = *reinterpret_cast<float*>(channel.ptr + channel.step * frame);
                sample = 0.f;
            }
        }
        
        // Perform mixing
        {
            std::lock_guard<std::mutex> lock(mFinalMixMutex);
            for(std::vector<Sample>::iterator iter = mFinalMix.begin(); iter != mFinalMix.end(); ++ iter) {
                Sample sample = *iter;
                
                sample.mix(mRuntime, channels, channelCount, frameCount, sampleRate);
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(mRuntimeMutex);
            mRuntime += frameDuration * frameCount;
        }
        soundio_outstream_end_write(stream);
        framesRemaining -= frameCount;
    }
}

void Endpoint::grabReciever(Receiver* receiver) {
    mReceivers.push_back(receiver);
    receiver->grab();
}

void Endpoint::dropReceiver(Receiver* receiver) {
    assert(mReceivers.find(receiver) != mReceivers.end() && "Attempted to drop endpoint from receiver which had not yet grabbed it");
    
    mReceivers.erase(std::remove(mReceivers.begin(), mReceivers.end(), receiver), mReceivers.end());
    receiver->drop();
}

void Endpoint::update(double time) {
    for(std::vector<Receiver*>::iterator iter = mReceivers.begin(); iter != mReceivers.end(); ++ iter) {
        Receiver* receiver = *iter;
        if(receiver->mRequestedContext || receiver->mRequestedEndpoint) {
            
        } else {
            receiver->updateCalc(time, this);
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(mFinalMixMutex);
        for(std::vector<Receiver*>::iterator iter = mReceivers.begin(); iter != mReceivers.end(); ++ iter) {
            Receiver* receiver = *iter;
            receiver->updateSafe(time, this);
        }
    }
}

double Endpoint::getRuntime() {
    std::lock_guard<std::mutex> lock(mRuntimeMutex);
    return mRuntime;
}

void Endpoint::syncRuntime() {
    std::lock_guard<std::mutex> lock(mRuntimeMutex);
    mRuntime = PepperGrains::getSingleton()->getRunningTimeSeconds();
}

void Endpoint::playWaveform(Waveform* waveform) {
    waveform->grab();
    mDirectWaveforms.push_back(waveform);
}

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames) {
    Endpoint* endpnt = static_cast<Endpoint*>(stream->userdata);
    endpnt->writeCallback(stream, minFrames, maxFrames);
}

} // namespace Sound
} // namespace pgg
