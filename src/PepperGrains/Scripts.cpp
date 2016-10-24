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
    
    RegRef mSandboxEnv = LUA_NOREF;
    
    void init() {
        assert(!mL && "The Lua state has already been created!");
        
        mL = luaL_newstate();
        luaL_openlibs(mL);
        
        // Setup sandboxes
        assert(mSandboxEnv == LUA_NOREF && "The Lua sandbox has already been created!");
        lua_newtable(mL);
        mSandboxEnv = luaL_ref(mL, LUA_REGISTRYINDEX);
        lua_rawgeti(mL, LUA_REGISTRYINDEX, mSandboxEnv);
        
        lua_pushstring(mL, "_G");
        lua_rawgeti(mL, LUA_REGISTRYINDEX, mSandboxEnv);
        lua_settable(mL, -3);
        
        lua_pushstring(mL, "print");
        lua_getglobal(mL, "print");
        lua_settable(mL, -3);
        
        lua_pushstring(mL, "pairs");
        lua_getglobal(mL, "pairs");
        lua_settable(mL, -3);
        
        lua_pushstring(mL, "ipairs");
        lua_getglobal(mL, "ipairs");
        lua_settable(mL, -3);
    }
    
    lua_State* getState() {
        assert(mL && "The Lua state has not yet been created!");
        return mL;
    }
    
    RegRef loadFile(const char* filename, bool sandbox) {
        /* LUA_OK = success
         * LUA_ERRSYNTAX = syntax error
         * LUA_ERRMEM = out of memory
         * LUA_ERRGCMM = error running __gc metamethod (produced by garbage collector)
         * LUA_ERRFILE = file cannot be open or read
         */
        int status = luaL_loadfilex(mL, filename, "t"); // -0 +1 m
        
        if(status == LUA_OK) {
            if(sandbox) {
                //lua_getglobal(mL, "_G");
                lua_rawgeti(mL, LUA_REGISTRYINDEX, mSandboxEnv);
                assert(std::string(lua_setupvalue(mL, -2, 1)) == "_ENV" && "First Lua upvalue is not _ENV; sandboxing failed!");
            }
            
            return luaL_ref(mL, LUA_REGISTRYINDEX); // -1 +0 -
        }
    }
    
    void unref(RegRef& ref) {
        // LUA_NOREF and LUA_REFNIL do nothing
        luaL_unref(mL, LUA_REGISTRYINDEX, ref); // -0 +0 -
        ref = LUA_NOREF;
    }
    
    void pushFunc(RegRef ref) {
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
            Logger::log(Logger::WARN) << "Error running Lua script!" << std::endl << lua_tostring(mL, -1) << std::endl;
            // TODO error instead
            lua_pop(mL, 1); // -1 +0
            return false;
        }
    }
    
    void close() {
        assert(mL && "The Lua state cannot be unloaded before creation!");
        luaL_unref(mL, LUA_REGISTRYINDEX, mSandboxEnv);
        mSandboxEnv = LUA_NOREF;
        lua_close(mL);
    }

} // Scripts
} // pgg
