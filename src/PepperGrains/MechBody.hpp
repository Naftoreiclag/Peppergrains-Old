#ifndef PGG_MECHBODY_HPP
#define PGG_MECHBODY_HPP

#include <vector>
#include <stdint.h>

namespace pgg {

class MechBody {
private:
    class Plate {
        
        Plate();
        virtual ~Plate();
        
        int32_t mLocX;
        int32_t mLocY;
        int32_t mLocZ;
        
        int32_t mScaleX;
        int32_t mScaleY;
        int32_t mScaleZ;
    };
    
public:
    MechBody();
    ~MechBody();
    
    std::vector<Plate*> mPlates;
};

}

#endif // PGG_MECHBODY_HPP
