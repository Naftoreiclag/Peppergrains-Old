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

#include "Scripts.hpp"

#include <cassert>

#include "Logger.hpp"

namespace pgg {
namespace Scripts {

    lua_State* mL = nullptr;
    
    void init() {
        assert(!mL && "The Lua state has already been created!");
        
        mL = luaL_newstate();
        luaL_openlibs(mL);
    }
    
    lua_State* getState() {
        assert(mL && "The Lua state has not yet been created!");
        return mL;
    }
    
    FuncRef loadFile(const char* filename) {
        /* LUA_OK = success
         * LUA_ERRSYNTAX = syntax error
         * LUA_ERRMEM = out of memory
         * LUA_ERRGCMM = error running __gc metamethod (produced by garbage collector)
         * LUA_ERRFILE = file cannot be open or read
         */
        int status = luaL_loadfilex(mL, filename, "t"); // -0 +1 m
        
        if(status == LUA_OK) {
            return luaL_ref(mL, LUA_REGISTRYINDEX); // -1 +0 -
        }
    }
    
    void unload(FuncRef ref) {
        // LUA_NOREF and LUA_REFNIL do nothing
        luaL_unref(mL, LUA_REGISTRYINDEX, ref); // -0 +0 -
    }
    
    void pushFunc(FuncRef ref) {
        lua_rawgeti(mL, LUA_REGISTRYINDEX, ref); // -0 +1 -
    }
    
    bool callFunc(int nargs, int nresults) {
        /* LUA_OK = success
         * LUA_ERRRUN = runtime error
         * LUA_ERRMEM = out of memory (message handler not called)
         * LUA_ERRERR = error in message handler
         * LUA_ERRGCMM = error running __gc metamethod (produced by garbage collector)
         */
        
        int status = lua_pcall(mL, nargs, nresults, 0); // -(nargs+1) +(nresults|1) -
        
        if(status == LUA_OK) {
            return true;
        } else {
            Logger::log(Logger::WARN) << "Error running Lua script!" << std::endl;
            // TODO error instead
            lua_pop(mL, 1); // -1 +0
            return false;
        }
    }
    
    void close() {
        assert(mL && "The Lua state cannot be unloaded before creation!");
        lua_close(mL);
    }

} // Scripts
} // pgg
