#pragma once
#include <vector>


namespace vektori {
 
    float pistetulo(std::vector<float> a, std::vector<float> b);
    std::vector<float> miksaus(std::vector<float> a, std::vector<float> b, float maara);

    std::vector<float> summa(std::vector<float> a, std::vector<float> b);
    float summa(std::vector<float> a);

    std::vector<float> erotus(std::vector<float> a, std::vector<float> b);

    std::vector<float> jako(std::vector<float> a, float jakaja);
    
    float pituus(std::vector<float> a);
}
