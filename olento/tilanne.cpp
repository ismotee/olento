#include "tilanne.h"
#include <cmath>
#include <iostream>

std::vector<float> stovf(std::string line) {

    std::vector<float> result;
    std::stringstream ss;
    float value;
    ss.precision(6);
    ss.str(line);

    while (ss >> std::fixed >> value) {
        result.push_back(value);
    }
    return result;
}

tilanne::tilanne(std::vector<float> in, std::vector<float> out) {
    input = in;
    output = out;

    populations.clear();

    for (int i = 0; i < output.size(); i++) {
        populations.push_back(genetic());
        populations.back().make_population(30, input.size(), 30);
        populations.back().evaluate_fitness(input, output[i]);
    }

}

tilanne::tilanne(std::string data) {

    std::stringstream ss;

    input.clear();
    output.clear();

    ss.str(data);
    std::string the_line;

    std::getline(ss, the_line);
    input = stovf(the_line);

    std::getline(ss, the_line);
    output = stovf(the_line);

    populations.clear();

    for (int i = 0; i < output.size(); i++) {
        populations.push_back(genetic());
        populations.back().make_population(30, input.size(), 30);
        populations.back().evaluate_fitness(input, output[i]);
    }
}

float tilanne::compare(std::vector<float> inp) {
    float result = 0;

    if (inp.size() != input.size()) {
        std::cout << "tilanne compare: vaara vectori koko: "<< inp.size() << "/" << input.size() <<"\n";
        return -1;
    }

    for (int i = 0; i < input.size(); i++) {
        result += (float) std::abs(input[i] - inp[i]);
    }
    return result;
}

float tilanne::compare_one(int id, float inp) {
    if(id < input.size())
        return (float) std::abs(input[id] - inp);
    
    return 0;
}

std::string tilanne::toString() {
    std::stringstream ss;

    ss.precision(6);
    
    for (int i = 0; i < input.size(); i++)
        ss << std::fixed << input[i];
        ss << " ";

    ss << "\n";

    for (int i = 0; i < output.size(); i++) {
        ss << std::fixed << output[i];
        ss << " ";
    }
    
    ss << "\n";

    return ss.str();

}

void tilanne::epoch() {

    if (!input.empty()) {

        for (int i = 0; i < populations.size(); i++) {
            populations[i].evaluate_fitness(input, output[i]);

            int iterations = 0;
            while (populations[i].get_best_fitness() > 0.03f && iterations < 70) {
                populations[i].mutate(5);
                populations[i].make_offspring();
                populations[i].evaluate_fitness(input, output[i]);
                iterations++;
            }
            std::cout << "pop " << i << ": " << iterations << "\n";

        }
    } else {
        std::cout << "bad input vector size\n";
    }
}

std::vector<float> tilanne::annaGeneettinenOutput(std::vector<float> numbers) {
    std::vector<float> result;

    for(int i = 0; i < populations.size();i++)
        result.push_back(populations[i].get_best_dna().count(numbers));
    
    return result;
}

void tilanne::jarjestaGeenit(std::vector<float> inp) {
    for(int i = 0; i < populations.size(); i++) {
        populations[i].evaluate_fitness(inp,output[i]);
    }
}

float tilanne::compare_out(std::vector<float> out) {
    float result = 0;

    if (out.size() != output.size()) {
        std::cout << "tilanne compare out: vaara vectori koko: "<< out.size() << "/" << output.size() <<"\n";
        return -1;
    }

    for (int i = 0; i < output.size(); i++) {
        result += (float) std::abs(output[i] - out[i]);
    }
    return result;
    
}