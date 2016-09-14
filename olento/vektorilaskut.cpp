#include "vektorilaskut.h"

namespace vektori {
    
    std::vector<float> erotus(std::vector<float> a, std::vector<float> b) {        
        if(a.size() != b.size())
            return std::vector<float>();
        
        std::vector<float> result(a.size(),0);
        
        for(int i = 0; i < a.size(); i++) {
            result[i] = b[i] - a[i];
        }
        
        return result;
    }
    
    float pistetulo(std::vector<float> a, std::vector<float> b) {
        if(a.size() != b.size())
            return 0;
        
        float result = 0;
        
        for(int i = 0; i < a.size(); i++) {
            result += b[i] * a[i];
        }        
        
        return result;
    }
    
    std::vector<float> miksaus(std::vector<float> a, std::vector<float> b, float maara) {
        if(a.size() != b.size())
            return std::vector<float>();
        
        std::vector<float> result = erotus(a,b);
        
        for(int i = 0; i < a.size(); i++) {
            result[i] = a[i] + result[i] * maara;
        }
        
        return result;        
    }
    
}

