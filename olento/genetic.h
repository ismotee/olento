#pragma once

#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/*float randf(float min = 0, float max) {
    float rand_f = (float)(rand() % 100001) / 100000; 
    return rand_f * (max - min) + min; 
};
*/
class gene {

public:
    std::string symbol; 
    
    virtual std::string get_symbol(){return symbol;}
    virtual float count(float rhs) = 0;

};

class zero : public gene {
public:
    zero(){symbol = "0*";} 
    float count(float rhs) {return 0;}
};

class divide : public gene {
protected:
    float div;
public:
    divide(int div_, int max_dna_):div( (  (float)1 / max_dna_ * div_ - 0.5f) * 2){symbol = std::to_string(div) + "*";} 
    float count(float rhs) {return rhs*div;}
};

class dna {
protected:
    std::vector<divide> genes;
    std::vector<int> genes_str;
    int max_dna;
    
public:
    
    float fitness;
    int diversity;
    
    dna(){}
    dna(std::vector<int> genes_, int max_);


    void make_genes(std::vector<int> genes_);
    float count(std::vector<float> numbers);
    void count_fitness(std::vector<float> numbers,float ideal);
    void evaluate_diversity();
    void mutate(int amount);
    dna make_offspring(dna mother);

    std::string print(std::vector<float> numbers);
};

class genetic {
public:
    std::vector<dna> population;
    int max_dna;

    void make_population(int amount, int dna_lenght, int max_dna_id);
    void evaluate_fitness(std::vector<float> numbers,float ideal);
    void evaluate_diversity();
    void mutate(int amout);
    void make_offspring();
    
    float get_best_fitness();
    dna get_best_dna();
    std::string print(std::vector<float> numbers);
    
private:

};

