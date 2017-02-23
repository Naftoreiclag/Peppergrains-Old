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

#include <functional>
#include <stdint.h>

// Debug only
#include <iostream>

namespace pgg {
namespace Memory {

class GarbageCollection;
    

class AbstractGarbageHandle {
public:
    virtual ~AbstractGarbageHandle() { }
};

template <typename T> class GarbageHandle : public AbstractGarbageHandle {

public:

    /**
     * Shared data among all copies of an instance. Contains the number of copies and also a pointer to the
     * collection/factory which generated the object.
     * 
     * Also used to uniquely identify all copies of an instance.
     */
    struct Shared {
        
        /// The number of existing copies
        uint32_t mNumCopies;
        
        /**
         * The function used instead of "delete" if supplied.
         * If empty, then "delete" is used. (Of course)
         */
        std::function<void(T*)> const mDeleteFunc;
        
        Shared(uint32_t numCopies = 0)
        : mNumCopies(numCopies) {
        }
        
        Shared(std::function<void(T*)> deleteFunc, uint32_t numCopies = 0)
        : mDeleteFunc(deleteFunc)
        , mNumCopies(numCopies) {
        }
    };
    
private:
    
    /**
     * Shared data among all copies of this instance.
     */
    Shared* mShared;
    
    /**
     * The pointer to the underlying object. Can be accessed with -> operator or by calling getPtr().
     * Arguably, storing the pointer here increases speed and decreases binary size at the cost of higher runtime
     * memory usage.
     */
    T* mWrapped;
    
    void incrementCopyCount() {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tIncrement" << std::endl;
        if(!mShared) return;
        ++ mShared->mNumCopies;
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tNum copies: " << mShared->mNumCopies << std::endl;
    }
    
    /**
     * @brief Decreases the copy count by one. Calls owner cleanup method if copy counter reaches zero.
     */
    void decrementCopyCount() {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tDecrement" << std::endl;
        if(!mShared) return;
        -- mShared->mNumCopies;
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tNum copies: " << mShared->mNumCopies << std::endl;
        if(mShared->mNumCopies == 0) {
            if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tCalling cleanup..." << std::endl;
            
            if(mShared->mDeleteFunc) {
                mShared->mDeleteFunc(mWrapped);
            } else {
                delete mWrapped;
            }
            
            delete mShared;
        }
    }
    
public:
    
    /**
     * @brief Default constructor, initializes to nullptr
     */
    GarbageHandle()
    : mShared(nullptr)
    , mWrapped(nullptr) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tEmpty handle" << std::endl;
    }

    /**
     * @brief Used to construct the very first instance of this handle.
     * 
     * @param object The actual object being wrapped
     */
    GarbageHandle(T* object)
    : mShared(new Shared())
    , mWrapped(object) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tCreating new origin" << std::endl;
        incrementCopyCount();
    }
    
    /**
     * @brief Used to construct the very first instance of this handle, also supplying deleter function
     * 
     * @param deleteFunc Memory where instance-shared data will be held
     * @param object The actual object being wrapped
     */
    GarbageHandle(T* object, std::function<void(T*)> deleteFunc)
    : mShared(new Shared(deleteFunc))
    , mWrapped(object) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tCreating new origin" << std::endl;
        incrementCopyCount();
    }
    
    /// Copy constructor. Increments copy count.
    GarbageHandle(const GarbageHandle& rhs)
    : mShared(rhs.mShared)
    , mWrapped(rhs.mWrapped) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tCopy cstr" << std::endl;
        incrementCopyCount();
    }
    
    /// Move constructor. Increments copy count.
    GarbageHandle(GarbageHandle&& rhs)
    : mShared(rhs.mShared)
    , mWrapped(rhs.mWrapped) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tMove cstr" << std::endl;
        incrementCopyCount();
    }
    
    /**
     * @brief Equivalent to assignment
     * @param rhs The object to replace this one
     */
    void replace(const GarbageHandle& rhs) {
        if(rhs.mShared == mShared) return;
        decrementCopyCount();
        mShared = rhs.mShared;
        mWrapped = rhs.mWrapped;
        incrementCopyCount();
    }
    
    /**
     * @brief Releases the resource. Analogous to setting a pointer to null
     */
    void reset() {
        decrementCopyCount();
        mShared = nullptr;
        mWrapped = nullptr;
    }
    
    void isNull() {
        return mShared == nullptr;
    }
    
    /// Copy assignment operator
    GarbageHandle& operator=(const GarbageHandle& rhs) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tCopy assn" << std::endl;
        replace(rhs);
        
        return *this;
    }
    
    /// Move assignment operator
    GarbageHandle& operator=(GarbageHandle&& rhs) {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tMove assn" << std::endl;
        replace(rhs);
        
        return *this;
    }
    
    /// Destructor
    ~GarbageHandle() {
        if(mShared) std::cout << mShared; else std::cout << "0x_______"; std::cout << "\t" << this << "\tDstr" << std::endl;
        decrementCopyCount();
    }
    
    T* operator->() {
        return mWrapped;
    }
    
    T* getPtr() {
        return mWrapped;
    }
};

}
}

#endif // PGG_MEMORY_GARBAGEHANDLE_HPP

