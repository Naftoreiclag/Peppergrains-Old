# Peppergrains

Another perpetually incomplete "3D game engine."

## Description

Peppergrains is a personal project of mine, made specifically so I can develop
other applications that utilize it. I wanted to create an alternative to the
many other high-budget, free-to-use game engines that are out there. In my
opinion, most of those engines put too much emphasis on photorealistic graphics
and by design restrict the "types" of games that can be made with them. In
contrast, Peppergrains aims to favor higher performance, lower minimum system 
requirements, and easier game feature prototyping. The project is currently
deep in the "work-in-progress" phase, but hopefully someday I will actually
finish it.

Planned features include:
- Abstraction of multiplayer and singleplayer
- Built-in "modding" support through Lua
- An experimental spherical harmonics-based render system
- Component-system entity model
- Support for unconventional 3D models and rendering (tangent vector fields, 
  implicit surface, etc.)

### License

The Peppergrains engine source code, which is located in the `src/` directory,
is proudly licensed under the 
[Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).
A copy of this license is available in the `LICENSE` file located in the
root of this repository.

Files found outside the `src/` repository, including but not limited to those
located in the `cmake/` directory, may be subject to different licensing
terms. Please see individual files for details.

## Building & Running

Peppergrains uses [CMake](https://cmake.org/) for build configurations.

### Dependencies

Peppergrains requires the following libraries to be built correctly.

- [OpenGL](https://www.opengl.org/) or [Vulkan](https://www.khronos.org/vulkan/)
  for rendering
- [SDL2](https://www.libsdl.org/) or [GLFW](http://www.glfw.org/)
  for windowing and event handling
- [Lua](http://www.lua.org/)
  for scripting
- [libsoundio](http://libsound.io)
  for sound
- [Boost](http://www.boost.org/)
  for all-around usefulness
- [Bullet Physics](http://bulletphysics.org/)
  for 3D collision resolution, 3D physics
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp/)
  for serialization, config files
- [stb](https://github.com/nothings/stb)
  for image loading

*Each library listed above is the copyright of its respective author(s). Please
see individual library homepages for more accurate licensing information.*

### Runtime Environment

By default, Peppergrains expects its runtime environment to be set up such that:
- `<home>/user/` contains all player-specific data
- `<home>/user/addons` contains all addons that can be loaded
- `<home>/resources/` contains all essential engine resources

`<home>` is by default `..`.

