#include "genetic.h"
#include <sstream>
#include <iostream>
#include <algorithm>

bool sort_dna(dna dna1, dna dna2) {
    return dna1.fitness > dna2.fitness;
}

bool sort_dna_by_diversity(dna dna1, dna dna2) {
    return dna1.diversity > dna2.diversity;
}

dna::dna(std::vector<int> genes_, int max_) : max_dna(max_) {
    make_genes(genes_);
}

void dna::make_genes(std::vector<int> genes_) {

    genes_str = genes_;

    for (int i = 0; i < genes_.size(); i++) {

        /*if (genes_[i] == 0) {
            gene* gen = new zero();
            genes.push_back(gen);
        } else {
            gene* gen = new divide(genes_[i]);
            genes.push_back(gen);
        }*/
        genes.push_back(divide(genes_[i], max_dna));

    }

}

float dna::count(std::vector<float> numbers) {
    float result = 0;

    if (genes.size() != numbers.size()) {
        std::cout << "wrong size of number for genes:" << genes.size() << ":" << numbers.size() << "\n";
        return result;
    }

    for (int i = 0; i < genes.size(); i++) {
        result += genes[i].count(numbers[i]);
    }
    return result;
}

std::string dna::print(std::vector<float> numbers) {
    std::stringstream result;

    if (genes.size() != numbers.size()) {
        std::cout << "wrong vector size:" << genes.size() << ":" << numbers.size() << "\n";
        return "";
    }

    for (int i = 0; i < genes.size(); i++) {
        result << "+";
        result << genes[i].get_symbol();
        result << numbers[i];
    }


    return result.str();
}

void dna::count_fitness(std::vector<float> numbers, float ideal) {

    fitness = std::abs(ideal - count(numbers));
}

void dna::mutate(int amount) {
    for (int i = 0; i < amount; i++)
        genes_str[rand() % (genes_str.size() - 1)] = rand() % (max_dna - 1);
}

dna dna::make_offspring(dna mother) {
    std::vector<int> offspring_string;

    if (genes_str.size() != mother.genes_str.size() || genes_str.size() == 0) {
        std::cout << "wrong size dna in offspring: " << genes_str.size() << "/" << mother.genes_str.size() << "\n";
        return mother;
    }

    int split = rand() % (genes_str.size() / 3) + genes_str.size() / 3;
    offspring_string.insert(offspring_string.begin(), genes_str.begin(), genes_str.begin() + split);
    offspring_string.insert(offspring_string.end(), mother.genes_str.begin() + split, mother.genes_str.end());

    return dna(offspring_string, max_dna);
}

void dna::evaluate_diversity() {
    diversity = 0;

    for (int i = 0; i < max_dna; i++) {

        int amount = 0;

        for (int j = 0; j < genes_str.size(); j++) {
            if (genes_str[j] == i) amount++;
        }

        diversity += amount * 2;
    }
}

void genetic::make_population(int amount, int dna_lenght, int max_dna_id) {
    population.clear();
    max_dna = max_dna_id;
    for (int i = 0; i < amount; i++) {
        std::vector<int> dna_string;
        dna_string.resize(dna_lenght, 0);

        for (int j = 0; j < dna_lenght; j++) {
            dna_string[j] = rand() % max_dna;

        }

        population.push_back(dna(dna_string, max_dna));
    }
}

void genetic::evaluate_fitness(std::vector<float> numbers, float ideal) {

    for (int i = 0; i < population.size(); i++) {
        population[i].count_fitness(numbers, ideal);
    }

    std::sort(population.begin(), population.end(), sort_dna);

}

void genetic::mutate(int amount) {
    for (int i = 0; i < population.size(); i++) {
        population[i].mutate(amount);
    }
}

void genetic::make_offspring() {
    dna father;
    dna mother;
    std::vector<dna> new_offspring;
    std::vector<dna> sorted_by_diversity;

    evaluate_diversity();

    sorted_by_diversity = population;

    std::sort(sorted_by_diversity.begin(), sorted_by_diversity.end(), sort_dna_by_diversity);

    new_offspring.push_back(get_best_dna());

    while (new_offspring.size() < population.size()) {

        father = population.back();

        for (int i = 0; i < population.size(); i++) {
            if (rand() % population.size() < i + 1) {
                father = population[i];
                break;
            }

        }

        mother = sorted_by_diversity.back();


        for (int i = 0; i < sorted_by_diversity.size(); i++) {
            if (rand() % sorted_by_diversity.size() < i * 0.5 + 1) {
                mother = sorted_by_diversity[i];
                break;
            }
        }
        new_offspring.push_back(father.make_offspring(mother));
    }
    population = new_offspring;
}

void genetic::evaluate_diversity() {
    for (int i = 0; i < population.size(); i++)
        population[i].evaluate_diversity();
}

std::string genetic::print(std::vector<float> numbers) {
    std::stringstream ss;

    for (int i = 0; i < population.size(); i++) {
        ss << population[i].print(numbers) << " = " << population[i].count(numbers);
        ss << "\n";
    }
    return ss.str();
}

float genetic::get_best_fitness() {
    return population.back().fitness;
}

dna genetic::get_best_dna() {
    return population.back();
}