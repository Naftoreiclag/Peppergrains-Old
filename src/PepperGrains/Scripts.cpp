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

#include <algorithm>
#include <cassert>
#include <vector>

#include "Logger.hpp"

namespace pgg {
namespace Scripts {

    lua_State* mL = nullptr;
    
    RegRef mSandboxEnv = LUA_NOREF;
    RegRef mLuaVersion = LUA_NOREF;
    
    std::vector<std::string> mGImport = {
        "_VERSION",
        "assert",
        "coroutine.create",
        "coroutine.resume",
        "coroutine.running",
        "coroutine.status",
        "coroutine.wrap",
        "coroutine.yield",
        "error",
        "ipairs",
        "math.abs",
        "math.acos",
        "math.asin",
        "math.atan",
        "math.atan2",
        "math.ceil",
        "math.cos",
        "math.cosh",
        "math.deg",
        "math.exp",
        "math.floor",
        "math.fmod",
        "math.frexp",
        "math.huge",
        "math.ldexp",
        "math.log",
        "math.log10",
        "math.max",
        "math.min",
        "math.modf",
        "math.pi",
        "math.pow",
        "math.rad",
        "math.random",
        "math.sin",
        "math.sinh",
        "math.sqrt",
        "math.tan",
        "math.tanh",
        "next",
        "os.clock",
        "os.difftime",
        "os.time",
        "pairs",
        "pcall",
        "select",
        "string.byte",
        "string.char",
        "string.find",
        "string.format",
        "string.gmatch",
        "string.gsub",
        "string.len",
        "string.lower",
        "string.match",
        "string.reverse",
        "string.sub",
        "string.upper",
        "table.insert",
        "table.maxn",
        "table.remove",
        "table.sort",
        "tonumber",
        "tostring",
        "type",
        "unpack",
        "xpcall"
    };
    
    // li = Lua Interface
    int li_print(lua_State* ww) {
        Logger::Out out = Logger::log(Logger::INFO);
        out << "print" << std::endl;
        
        /*
        int numArgs = lua_gettop(ww);
        
        Logger::Out out = Logger::log(Logger::INFO);
        
        size_t len;
        for(int i = 1; i <= numArgs; ++ i) {
            out << std::string(luaL_tolstring(ww, i, &len), len); // equivalent to calling tostring() on that value in the stack
            if(i != numArgs) {
                out << '\t';
            }
            lua_pop(ww, 1); // luaL_tolstring also pushes the result onto the stack, but we don't need that
        }
        
        out << std::endl;
        */
        
        return 0; // print() returns nothing
    }
    
    void init() {
        assert(!mL && "The Lua state has already been created!");
        
        mL = luaL_newstate();
        luaL_openlibs(mL);
        
        assert(mLuaVersion == LUA_NOREF && "Lua _VERSION already acquired!");
        assert(lua_getglobal(mL, "_VERSION") == LUA_TSTRING && "Lua _VERSION missing!");
        Logger::log(Logger::INFO) << "Lua version: " << lua_tostring(mL, -1) << std::endl;
        mLuaVersion = luaL_ref(mL, LUA_REGISTRYINDEX);
        
        // Setup sandboxes
        assert(mSandboxEnv == LUA_NOREF && "The Lua sandbox has already been created!");
        lua_newtable(mL);
        /* -1 table (_ENV)
         */
        
        std::vector<std::string> createdModules;
        for(const std::string& wlEntry : mGImport) {
            std::string::size_type dot = wlEntry.find('.');
            if(dot != std::string::npos) {
                std::string module = wlEntry.substr(0, dot);
                std::string func = wlEntry.substr(dot + 1);
                // Sandboxed module not yet created
                if(std::find(createdModules.begin(), createdModules.end(), module) == createdModules.end()) {
                    lua_newtable(mL); // Create a new table
                    /* -2 table (_ENV)
                     * -1 table (_ENV.module)
                     */
                    lua_setfield(mL, -2, module.c_str()); // Set _ENV.module to the new table
                    /* -1 table (_ENV)
                     */
                    createdModules.push_back(module.c_str()); // Remember this module has been created
                    Logger::log(Logger::VERBOSE) << "Sandbox added new module: " << module << std::endl;
                }
                lua_getfield(mL, -1, module.c_str()); // Push _ENV.module
                /* -2 table (_ENV)
                 * -1 table (_ENV.module)
                 */
                lua_getglobal(mL, module.c_str()); // Push _G.module
                /* -3 table (_ENV)
                 * -2 table (_ENV.module)
                 * -1 table (_G.module)
                 */
                lua_getfield(mL, -1, func.c_str()); // Push _G.module.func
                /* -4 table (_ENV)
                 * -3 table (_ENV.module)
                 * -2 table (_G.module)
                 * -1 func  (_G.module.func)
                 */
                lua_remove(mL, -2); // Pop _G.module
                /* -3 table (_ENV)
                 * -2 table (_ENV.module)
                 * -1 func  (_G.module.func)
                 */
                lua_setfield(mL, -2, func.c_str()); // Set _ENV.module.func to _G.module.func
                /* -2 table (_ENV)
                 * -1 table (_ENV.module)
                 */
                lua_pop(mL, 1); // Pop top value (remove _ENV.module from stack)
                /* -1 table (_ENV)
                 */
                Logger::log(Logger::VERBOSE) << "Sandbox added: " << module << "." << func << std::endl;
            } else {
                lua_getglobal(mL, wlEntry.c_str()); // Push _G.wlEntry
                /* -2 table (_ENV)
                 * -1 value (_G.wlEntry)
                 */
                lua_setfield(mL, -2, wlEntry.c_str()); // Set _ENV.wlEntry to _G.wlEntry
                /* -1 table (_ENV)
                 */
                Logger::log(Logger::VERBOSE) << "Sandbox added: " << wlEntry << std::endl;
            }
        }
        
        lua_pushcfunction(mL, li_print);
        //lua_getglobal(mL, "print");
        /* -2 table (_ENV)
         * -1 cfunc (li_print)
         */
        lua_setfield(mL, -2, "print");
        /* -1 table (_ENV)
         */
        
        mSandboxEnv = luaL_ref(mL, LUA_REGISTRYINDEX);
        /* Lua stack balanced
         */
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
        luaL_unref(mL, LUA_REGISTRYINDEX, mLuaVersion);
        mLuaVersion = LUA_NOREF;
        lua_close(mL);
    }

} // Scripts
} // pgg
