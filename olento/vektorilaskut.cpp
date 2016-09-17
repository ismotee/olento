#include "vektorilaskut.h"
#include <math.h>
#include <iostream>

namespace vektori {
    
    
    float pistetulo(std::vector<float> a, std::vector<float> b) {
        if(a.size() != b.size()) {
            std::cout << "vektori pistetulo: väärä vektorin koko" << a.size() << "/" << b.size() << "\n";

            return 0;
        }
        float result = 0;
        
        for(int i = 0; i < a.size(); i++) {
            result += b[i] * a[i];
        }        
        
        return result;
    }
    
    std::vector<float> miksaus(std::vector<float> a, std::vector<float> b, float maara) {
        if(a.size() != b.size()) {
            std::cout << "vektori miksaus: väärä vektorin koko" << a.size() << "/" << b.size() << "\n";
            return std::vector<float>();
        }
        
        std::vector<float> result = erotus(a,b);
        
        for(int i = 0; i < a.size(); i++) {
            result[i] = a[i] + result[i] * maara;
        }
        
        return result;        
    }
    
    float summa(std::vector<float> a) {
        float result = 0;
        for(int i =0; i < a.size();i++)
            result += a[i];
        
        return result;
    }
    
    std::vector<float> summa(std::vector<float> a, std::vector<float> b) {
        if(a.size() != b.size()) {
            std::cout << "vektori summa: väärä vektorin koko" << a.size() << "/" << b.size() << "\n";
            return std::vector<float>();
        }
        
        std::vector<float> result = a;
        for(int i = 0; i < result.size(); i++)
            result[i] += b[i];
        
        return result;
    }

    std::vector<float> erotus(std::vector<float> a, std::vector<float> b) {        
        if(a.size() != b.size()) {
            std::cout << "vektori erotus: väärä vektorin koko" << a.size() << "/" << b.size() << "\n";
            return std::vector<float>();
        }
        
        std::vector<float> result(a.size(),0);
        
        for(int i = 0; i < a.size(); i++) {
            result[i] = b[i] - a[i];
        }
        
        return result;
    }

    std::vector<float> jako(std::vector<float> a, float jakaja) {
        if(jakaja == 0)
            return a;
        
        
        for(int i = 0; i < a.size(); i++)
            a[i] /= jakaja;
        
        return a;
    }

    float pituus(std::vector<float> a) {
        float result = sqrtf(pistetulo(a,a));
        return result;
    } 
    
}

