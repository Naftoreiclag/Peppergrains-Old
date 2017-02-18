/*
   Copyright 2017 James Fong

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

#ifndef PGG_MEMORY_GARBAGEHANDLE_HPP
#define PGG_MEMORY_GARBAGEHANDLE_HPP

#include <stdint.h>

namespace pgg {
namespace Memory {
    
struct SharedGarbageData {
    int32_t mNumCopies = 1;
};

class AbstractGarbageHandle {
    virtual ~AbstractGarbageHandle();
    virtual int32_t getNumCopies() = 0;
};

template <typename T> class GarbageHandle : public AbstractGarbageHandle {
private:
    
    SharedGarbageData* mShared;
    T* mWrapped;
    
public:
    /// Constructor
    GarbageHandle(T* object)
    : mShared(new SharedGarbageData())
    , mWrapped(object) {
    }
    
    /// Copy constructor
    GarbageHandle(const GarbageHandle& rhs)
    : mShared(rhs.mShared)
    , mWrapped(rhs.mWrapped) {
        ++ mShared->mNumCopies;
    }
    
    /// Move constructor
    GarbageHandle(GarbageHandle&& rhs)
    : mShared(rhs.mShared)
    , mWrapped(rhs.mWrapped) {
        ++ mShared->mNumCopies;
    }
    
    /// Copy assignment operator
    GarbageHandle& operator=(const GarbageHandle& rhs) {
        -- mShared->mNumCopies;
        mShared = rhs.mShared;
        mWrapped = rhs.mWrapped;
        ++ mShared->mNumCopies;
    }
    
    /// Move assignment operator
    GarbageHandle& operator=(GarbageHandle&& rhs) {
        -- mShared->mNumCopies;
        mShared = rhs.mShared;
        mWrapped = rhs.mWrapped;
        ++ mShared->mNumCopies;
    }
    
    /// Destructor
    ~GarbageHandle() {
        -- mShared->mNumCopies;
    }
    
    T* operator->() {
        return mWrapped;
    }
    
    int32_t getNumCopies() {
        return mShared->mNumCopies;
    }
};

}
}

#endif // PGG_MEMORY_GARBAGEHANDLE_HPP

