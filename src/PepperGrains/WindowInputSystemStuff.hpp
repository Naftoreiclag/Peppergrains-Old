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

#ifndef PGG_WINDOWINPUTSYSTEMSTUFF_HPP
#define PGG_WINDOWINPUTSYSTEMSTUFF_HPP

#ifdef PGG_SDL
#include <SDL2/SDL.h>
#endif

#ifdef PGG_GLFW

// Ensure glew.h is loaded before gl.h if we are using that
#ifdef PGG_OPENGL
#include <GraphicsApiStuff.hpp>
#endif

#include <GLFW/glfw3.h>
#endif

#endif // PGG_WINDOWINPUTSYSTEMSTUFF_HPP
