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
# - SOUNDIO_FOUND
# - SOUNDIO_INCLUDE_DIR
# - SOUNDIO_LIBRARY
# - SOUNDIO_VERSION_MAJOR
# - SOUNDIO_VERSION_MINOR
# - SOUNDIO_VERSION_PATCH
# - SOUNDIO_VERSION_STRING

set(SOUNDIO_FOUND FALSE)
find_path(SOUNDIO_INCLUDE_DIR NAMES "soundio/soundio.h")
set(SOUNDIO_VERSION_MAJOR "SOUNDIO_VERSION_MAJOR-NOTFOUND"
    CACHE STRING "Soundio major version")
set(SOUNDIO_VERSION_MINOR "SOUNDIO_VERSION_MINOR-NOTFOUND"
    CACHE STRING "Soundio minor version")
set(SOUNDIO_VERSION_PATCH "SOUNDIO_VERSION_PATCH-NOTFOUND"
    CACHE STRING "Soundio patch version")
find_library(SOUNDIO_LIBRARY NAMES soundio)

if(SOUNDIO_INCLUDE_DIR)
    set(SOUNDIO_FOUND TRUE)
endif()

set(SOUNDIO_VERSION_STRING
    "${SOUNDIO_VERSION_MAJOR}.${SOUNDIO_VERSION_MINOR}.${SOUNDIO_VERSION_PATCH}")

mark_as_advanced(
    SOUNDIO_INCLUDE_DIR
    SOUNDIO_VERSION_MAJOR
    SOUNDIO_VERSION_MINOR
    SOUNDIO_VERSION_PATCH
)

