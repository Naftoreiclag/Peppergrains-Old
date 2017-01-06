/*
   Copyright 2015-2017 James Fong

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

#ifndef PGG_GRAPHICSAPILIBRARY_HPP
#define PGG_GRAPHICSAPILIBRARY_HPP

/* Properly includes either:
 *      OPENGL
 *      VULKAN
 */

#ifdef PGG_OPENGL
#include <GL/glew.h>
#endif

#ifdef PGG_VULKAN
#include <vulkan/vulkan.h>

// Temporary quick fix
#include <stdint.h>
#define GLuint uint32_t
#define GLubyte uint8_t
#define GLint int32_t
#define GLbyte int8_t
#define GLfloat float
#define GLbool bool
#define GLenum int
#define GLchar char

#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#endif // PGG_GRAPHICSAPILIBRARY_HPP
