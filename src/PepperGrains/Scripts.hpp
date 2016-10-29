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

#ifndef PGG_ENGINESCRIPTS_HPP
#define PGG_ENGINESCRIPTS_HPP

#include "lua.hpp"

/* Functions not beginning with "push" or "pop" are always balanced
 * 
 * 
 * 
 */

namespace pgg {
namespace Scripts {
    
    typedef int RegRef;
    extern const RegRef REF_EMPTY; // TODO? remove this
    typedef int ErrorCode;
    extern const ErrorCode ERR_OK; // Success
    extern const ErrorCode ERR_RUNTIME; // Runtime error
    extern const ErrorCode ERR_MEMORY; // Out of memory (message handler not called)
    extern const ErrorCode ERR_ERR; // Error in message handler
    extern const ErrorCode ERR_GCMETAMETHOD; // Error running __gc metamethod (produced by garbage collector)
    struct CallStat {
        ErrorCode error;
    };

    void init();
    lua_State* getState();
    
    RegRef loadFunc(const char* filename, RegRef env = LUA_NOREF);
    RegRef newEnvironment();
    
    void pushRef(RegRef ref); // Pushes a Lua value onto the stack, referenced in the registry by ref
    void pop(int n = 1); // Pops a value off the stack
    
    CallStat popCallFuncArgs(int nargs = 0, int nresults = 0); // Calls function with arguments on top of stack
    void setEnv(RegRef env); // Sets the environment of the value on the top of stack
    
    void unref(RegRef& ref);
    
    void close();
    
    // Allows scripts to use bootstrap-exclusive functions
    void enableBootstrap(bool enable = true);

} // Scripts
} // pgg

#endif // PGG_ENGINESCRIPTS_HPP
