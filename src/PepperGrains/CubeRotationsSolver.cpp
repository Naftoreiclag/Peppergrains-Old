/*
   Copyright 2016-2017 James Fong

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

#include "CubeRotationsSolver.hpp"

#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <cstdint>
#include <vector>

#include "OpenGLStuff.hpp"

namespace pgg {

namespace CubeRotationsSolver {

struct Orientation {
    bool x[8];
    bool y[8];
    bool z[8];
    
    std::vector<glm::quat> inputQuaternions;
};

// Are these two the same?
bool compareOrientations(Orientation* a, Orientation* b) {
    for(uint8_t i = 0; i < 8; ++ i) {
        if(a->x[i] != b->x[i]) {
            return false;
        }
        if(a->y[i] != b->y[i]) {
            return false;
        }
        if(a->z[i] != b->z[i]) {
            return false;
        }
    }
    
    return true;
}
    
void resolve() {
    
    std::vector<Orientation*> possibleOrientations;


    
    glm::quat rotation = glm::quat();
    for(uint32_t i = 0; i < 9999; ++ i) {
        
        // Perform the "logical" rotations first
        if(i < 24) {
            rotation = glm::quat();
            
            uint32_t rollType = i % 4;
            uint32_t secondaryType = (i - rollType) / 4;
            
            if(rollType == 0) {
                rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f)) * rotation;
            } else if(rollType == 1) {
                rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f)) * rotation;
            } else if(rollType == 2) {
                rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f)) * rotation;
            } else {
                rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.f, 0.f, 1.f)) * rotation;
            }
            
            if(secondaryType == 0) {
                // No other motion
            } else if(secondaryType == 1) {
                // Pitch
                rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * rotation;
            } else if(secondaryType == 2) {
                // Pitch
                rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f)) * rotation;
            } else if(secondaryType == 3) {
                rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f)) * rotation;
            } else if(secondaryType == 4) {
                rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)) * rotation;
            } else if(secondaryType == 5) {
                rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.f, 1.f, 0.f)) * rotation;
            }
        }
        else {
            uint32_t rAxis = std::rand() % 3;
            glm::vec3 axis = glm::vec3(0.f, 0.f, 0.f);
            if(rAxis == 0) {
                axis.x = 1.f;
            } else if(rAxis == 1) {
                axis.y = 1.f;
            } else {
                axis.z = 1.f;
            }

            uint32_t rAngle = std::rand() % 9;

            float angle;
            if(rAngle == 0) {
                angle = -360.f;
            } else if(rAngle == 1) {
                angle = -270.f;
            } else if(rAngle == 2) {
                angle = -180.f;
            } else if(rAngle == 3) {
                angle = -90.f;
            } else if(rAngle == 4) {
                angle = 0.f;
            } else if(rAngle == 5) {
                angle = 90.f;
            } else if(rAngle == 6) {
                angle = 180.f;
            } else if(rAngle == 7) {
                angle = 270.f;
            } else {
                angle = 360.f;
            }
            
            rotation = glm::angleAxis(glm::radians(angle), axis) * rotation;
            
            uint32_t reset = std::rand() % 100;
            if(reset == 0) {
                rotation = glm::quat();
            }
        }
        
        glm::mat4 rotMatr = glm::mat4_cast(rotation);
        
        Orientation* current = new Orientation();
        for(uint8_t j = 0; j < 8; ++ j) {
            glm::vec4 corner(
                (j & (1 << 0)) ? -1.f : 1.f,
                (j & (1 << 1)) ? -1.f : 1.f,
                (j & (1 << 2)) ? -1.f : 1.f,
                1.f
            );
            
            glm::vec4 result = rotMatr * corner;
            
            current->x[j] = result.x > 0.f;
            current->y[j] = result.y > 0.f;
            current->z[j] = result.z > 0.f;
        }
        bool addNew = true;
        for(std::vector<Orientation*>::iterator iter = possibleOrientations.begin(); iter != possibleOrientations.end(); ++ iter) {
            Orientation* potato = *iter;
            
            if(compareOrientations(current, potato)) {
                delete current;
                current = potato;
                addNew = false;
                break;
            }
        }
        if(addNew) {
            possibleOrientations.push_back(current);
        }
        
        current->inputQuaternions.push_back(rotation);
        
    }
    
    std::cout << possibleOrientations.size() << std::endl;
    
    for(std::vector<Orientation*>::iterator iter = possibleOrientations.begin(); iter != possibleOrientations.end(); ++ iter) {
        float ep = 0.00001f;
        
        Orientation* potato = *iter;
        
        for(uint8_t i = 0; i < 8; ++ i) {
            std::cout
            << ((i & (1 << 0)) ? "0" : "1")
            << ((i & (1 << 1)) ? "0" : "1")
            << ((i & (1 << 2)) ? "0" : "1")
            << "\t"
            << (potato->x[i] ? "1" : "0")
            << (potato->y[i] ? "1" : "0")
            << (potato->z[i] ? "1" : "0");
            
            if(i % 2 == 1) {
                std::cout << std::endl;
            } else {
                std::cout << "\t\t";
            }
        }
        
        {
            glm::vec4 front(0.f, 0.f, 1.f, 1.f);
            glm::mat4 rotMatr = glm::mat4_cast(*potato->inputQuaternions.begin());
            
            front = rotMatr * front;
            
            if(std::abs(front.x) < ep) {
                front.x = 0.f;
            }
            if(std::abs(front.y) < ep) {
                front.y = 0.f;
            }
            if(std::abs(front.z) < ep) {
                front.z = 0.f;
            }
            
            std::cout << front.x << "\t" << front.y << "\t" << front.z << std::endl;
            
        }
        
        std::cout << potato->inputQuaternions.size() << std::endl;
        std::vector<glm::quat> uniqueQuats;
        
        for(std::vector<glm::quat>::iterator quiter = potato->inputQuaternions.begin(); quiter != potato->inputQuaternions.end(); ++ quiter) {
            glm::quat inputQuat = *quiter;
            bool unique = true;
            for(std::vector<glm::quat>::iterator quiter2 = uniqueQuats.begin(); quiter2 != uniqueQuats.end(); ++ quiter2) {
                glm::quat uniqueQuat = *quiter2;
                
                if(std::abs(inputQuat.x - uniqueQuat.x) < ep && std::abs(inputQuat.y - uniqueQuat.y) < ep && std::abs(inputQuat.z - uniqueQuat.z) < ep && std::abs(inputQuat.w - uniqueQuat.w) < ep) {
                    unique = false;
                    break;
                }
            }
            
            if(unique) {
                uniqueQuats.push_back(inputQuat);
            }
        }
        
        for(std::vector<glm::quat>::iterator quiter2 = uniqueQuats.begin(); quiter2 != uniqueQuats.end(); ++ quiter2) {
            glm::quat egg = *quiter2;

            float x = egg.x;
            float y = egg.y;
            float z = egg.z;
            float w = egg.w;
            
            if(std::abs(x) < ep) {
                x = 0.f;
            }
            if(std::abs(y) < ep) {
                y = 0.f;
            }
            if(std::abs(z) < ep) {
                z = 0.f;
            }
            if(std::abs(w) < ep) {
                w = 0.f;
            }
            
            std::cout << "x:" << x << "\ty:" << y << "\tz:" << z << "\tw:" << w << std::endl;
        }
        
        
        std::cout << std::endl;
    }
    
    for(std::vector<Orientation*>::iterator iter = possibleOrientations.begin(); iter != possibleOrientations.end(); ++ iter) {
        delete *iter;
    }
}

}

}
