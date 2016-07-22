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
    
    int error = soundio_outstream_open(mStream);
    if(error) {
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

void Endpoint::writeCallback(SoundIoOutStream* stream, int minFrames, int maxFrames) {
}

void Endpoint::grabReciever(Receiver* receiver) {
    mReceivers.push_back(receiver);
}

void Endpoint::dropReceiver(Receiver* receiver) {
    mReceivers.erase(std::remove(mReceivers.begin(), mReceivers.end(), receiver), mReceivers.end());
    
    receiver->drop();
}
void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames) {
    Endpoint* endpnt = static_cast<Endpoint*>(stream->userdata);
    endpnt->writeCallback(stream, minFrames, maxFrames);
}

} // namespace Sound
} // namespace pgg
