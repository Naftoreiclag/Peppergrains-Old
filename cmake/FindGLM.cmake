#   Copyright 2017 James Fong
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

# Populates:
# - GLM_FOUND
# - GLM_INCLUDE_DIR
# - GLM_VERSION_MAJOR
# - GLM_VERSION_MINOR
# - GLM_VERSION_PATCH
# - GLM_VERSION_STRING

set(GLM_FOUND FALSE)
find_path(GLM_INCLUDE_DIR NAMES "glm/glm.hpp")
set(GLM_VERSION_MAJOR "GLM_VERSION_MAJOR-NOTFOUND"
    CACHE STRING "GLM major version")
set(GLM_VERSION_MINOR "GLM_VERSION_MINOR-NOTFOUND"
    CACHE STRING "GLM minor version")
set(GLM_VERSION_PATCH "GLM_VERSION_PATCH-NOTFOUND"
    CACHE STRING "GLM patch version")

if(GLM_INCLUDE_DIR)
    set(GLM_FOUND TRUE)
endif()

set(GLM_VERSION_STRING
    "${GLM_VERSION_MAJOR}.${GLM_VERSION_MINOR}.${GLM_VERSION_PATCH}")

mark_as_advanced(
    GLM_INCLUDE_DIR
    GLM_VERSION_MAJOR
    GLM_VERSION_MINOR
    GLM_VERSION_PATCH
)
