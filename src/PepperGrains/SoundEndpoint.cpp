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

ThreadData::ThreadData(double startTime)
: progress(startTime) {
}
    
PlayingWaveformInterface::PlayingWaveformInterface(Waveform* waveform, double cgt, double currentPos)
: mWaveform(waveform)
, mThreadData(new ThreadData(currentPos))
, mUpdateTimestamp(cgt)
, mProgressUpdate(currentPos)
, mStopAsap(false) {
    if(mWaveform) {
        mWaveform->grab();
    }
}
PlayingWaveformInterface::~PlayingWaveformInterface() {
    if(mWaveform) {
        mWaveform->drop();
    }
    delete mThreadData;
}
void PlayingWaveformInterface::load() { }
void PlayingWaveformInterface::unload() { delete this; }
PlayingWaveformInterface* PlayingWaveformInterface::reckonSpeed(double speed) {
    mSpeedReckon = speed;
    return this;
}
PlayingWaveformInterface* PlayingWaveformInterface::updateProgress(double cgt, double progress) {
    mUpdateTimestamp = cgt;
    mProgressUpdate = progress;
    return this;
}
void PlayingWaveformInterface::asapStop() {
    mStopAsap = true;
    mSpeedReckon = 0.f;
}

PlayingWaveform::PlayingWaveform(PlayingWaveformInterface* pwi)
: waveform(pwi->mWaveform)
, timestamp(pwi->mUpdateTimestamp)
, speedReckon(pwi->mSpeedReckon)
, progress(pwi->mProgressUpdate)
, threadData(pwi->mThreadData) {
}
    
Endpoint::Endpoint()
: mDevice(nullptr)
, mStream(nullptr) {
}

Endpoint::~Endpoint() {
    if(mStream) soundio_outstream_destroy(mStream);
    if(mDevice) soundio_device_unref(mDevice);
}

void Endpoint::setDevice(SoundIoDevice* device) {
    int error;
    
    if(mDevice) {
        soundio_device_unref(mDevice);
        if(mStream) soundio_outstream_destroy(mStream);
    }
    
    mDevice = device;
    if(!mDevice) {
        return;
    }
    
    soundio_device_ref(mDevice);
    
    mStream = soundio_outstream_create(mDevice);
    
    if(!mStream) {
        std::cerr << "Ran out of memory while trying to create sound output stream!" << std::endl;
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    
    mStream->format = SoundIoFormatFloat32NE;
    mStream->write_callback = endpointSoundIoWriteCallback;
    mStream->userdata = this;
    //mStream->sample_rate = soundio_device_nearest_sample_rate(mDevice, 1);
    
    error = soundio_outstream_open(mStream);
    if(error) {
        std::cout << "Sample rate: " << mStream->sample_rate << std::endl;
        std::cerr << "Error while opening audio stream: " << soundio_strerror(error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    if(mStream->layout_error) {
        std::cerr << "Error while setting channel layout: " << soundio_strerror(mStream->layout_error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    error = soundio_outstream_start(mStream);
    if(error) {
        std::cerr << "Error while starting audio stream: " << soundio_strerror(error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
}

void Endpoint::writeCallback(SoundIoOutStream* stream, uint32_t minFrames, uint32_t maxFrames) {
    
    double cgt = PepperGrains::getSingleton()->getRunningTimeMilliseconds();
    
    const SoundIoChannelLayout& layout = stream->layout;
    
    uint32_t framesRemaining = 1000;
    if(framesRemaining < minFrames) { framesRemaining = minFrames; }
    if(framesRemaining > maxFrames) { framesRemaining = maxFrames; }
    uint32_t sampleRate = stream->sample_rate;
    
    double frameDuration = 1.0 / sampleRate;
    
    SoundIoChannelArea* channels;
    uint32_t channelCount = layout.channel_count;
    
    while(framesRemaining > 0) {
        int frameCount = framesRemaining;
        int error = soundio_outstream_begin_write(stream, &channels, &frameCount);
        if(error) {
            // !!!
        }
        
        if(!frameCount) {
            break;
        }
        
        double chunkDuration = frameCount * frameDuration;
        
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
            std::lock_guard<std::mutex> lock(mSoundioThreadMutex);
            for(std::vector<PlayingWaveform>::iterator iter = mThreadWaveforms.begin(); iter != mThreadWaveforms.end(); ++ iter) {
                // Copying rather than reference arguably more efficient here
                PlayingWaveform pw = *iter;
                ThreadData* td = pw.threadData;
                
                double startX = cgt;
                double endX = startX + chunkDuration;
                
                double startY = td->progress;
                double endY = ((endX - pw.timestamp) * pw.speedReckon) + pw.progress;
                
                pw.waveform->mix(channels, channelCount, frameCount, startY, endY);
                td->progress = endY;
            }
        }
        soundio_outstream_end_write(stream);
        framesRemaining -= frameCount;
        cgt += chunkDuration;
    }
}

PlayingWaveformInterface* Endpoint::playWaveform(Waveform* waveform, double cgt, double startPos) {
    PlayingWaveformInterface* pwi = new PlayingWaveformInterface(waveform, cgt, startPos);
    
    // No need to intepret if there is no waveform
    if(waveform) {
        pwi->grab();
        mPlayingWaveforms.push_back(pwi);
    }
    
    return pwi;
}

void Endpoint::updateSoundThread() {
    bool needsDeletion = false;
    {
        std::lock_guard<std::mutex> lock(mSoundioThreadMutex);
        mThreadWaveforms.clear();
        for(std::vector<PlayingWaveformInterface*>::iterator iter = mPlayingWaveforms.begin(); iter != mPlayingWaveforms.end(); ++ iter) {
            PlayingWaveformInterface* pwi = *iter;
            if(pwi->mStopAsap) {
                needsDeletion = true;
                continue;
            }
            mThreadWaveforms.push_back(PlayingWaveform(pwi));
        }
    }
    if(needsDeletion) {
        for(size_t index = 0; index < mPlayingWaveforms.size();) {
            PlayingWaveformInterface* pwi = mPlayingWaveforms[index];
            if(pwi->mStopAsap) {
                std::swap(pwi, mPlayingWaveforms.back());
                mPlayingWaveforms.pop_back();
                
                pwi->drop();
            } else {
                ++ index;
            }
        }
    }
    
}

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames) {
    Endpoint* endpnt = static_cast<Endpoint*>(stream->userdata);
    endpnt->writeCallback(stream, minFrames, maxFrames);
}

} // namespace Sound
} // namespace pgg
