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

#include "DebugFPControllerEListe.hpp"

#include "InputMoveESignal.hpp"
#include "PhysicsLocationUpdateESignal.hpp"



namespace pgg {

DebugFPControllerEListe::DebugFPControllerEListe(Vec3 initialLocation)
: mAbsoluteLocation(initialLocation) {
}

DebugFPControllerEListe::~DebugFPControllerEListe() {
}

void DebugFPControllerEListe::onEntityExists(nres::Entity* entity) {
    
}
void DebugFPControllerEListe::onEntityDestroyed(nres::Entity* entity) {
    
}
void DebugFPControllerEListe::onEntityBroadcast(nres::Entity* entity, const ESignal* data) {
    if(data->getType() == ESignal::Type::INPUT_MOVE) {
        const InputMoveESignal* input = (const InputMoveESignal*) data;
        
        mAbsoluteLocation += input->mDisplacement - (Vec3(0, 1, 0) * input->mDisplacement.dot(Vec3(0, 1, 0)));
        
        entity->broadcast(new PhysicsLocationUpdateESignal(mAbsoluteLocation));
    }
}

}

