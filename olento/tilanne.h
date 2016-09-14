#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include "genetic.h"

class tilanne {

    std::vector<float> input;
    std::vector<float> output;
    std::vector<genetic> populations;

public:
    
    tilanne(){};
    tilanne(std::vector<float> in, std::vector<float> out);
    tilanne(std::string data);
    
    std::vector<float> getOutput() {return output;}
    std::vector<float> getInput() {return input;}
    float compare(std::vector<float> inp);
    float compare_one(int id,float inp);
    float compare_out(std::vector<float> out);
    
    void epoch();
    void jarjestaGeenit(std::vector<float> inp);
    
    std::vector<float> annaGeneettinenOutput (std::vector<float> numbers);
    
    std::string toString();

};

