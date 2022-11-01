#include <iostream>
#include <vector>
#include <functional>
#include <random>

//Przygotuj funkcję dekodującą genotyp w fenotyp. Niech genotyp będzie ciągiem bitów (vector<int> albo vector<bool>) długości
// (100+(twój numer indeksu modulo 10)*2 ). Rozwiązanie zadania to będzie wektor liczb zmiennoprzecinkowych. W zależności od wybranego
// zadania będzie to X,Y, albo X0, X1, ... Xn.

//


std::random_device rd;
std::mt19937 mt_generator(rd());

using chromosome_t = std::vector<int>;
using population_t = std::vector<chromosome_t>;

int genotype_len = 114;


auto genetic_algorithm = [](
        auto initial_population, auto fitness, auto term_condition,
        auto selection, double p_crossover,
        auto crossover, double p_mutation,  auto mutation) {
    using namespace std;
    uniform_real_distribution<double> uniform(0.0,1.0);
    auto population = initial_population;
    vector<double> population_fit = fitness(population);
    while (!term_condition(population,population_fit)) {
        auto parents_indexes = selection(population_fit);
        decltype(population) new_population;
        for (int i = 0 ; i < parents_indexes.size(); i+=2) {
            decltype(initial_population) offspring = {population[i],population[i+1]};
            if (uniform(mt_generator) < p_crossover) {
                offspring = crossover(offspring);
            }
            for (auto chromosome : offspring) new_population.push_back(chromosome);
        }
        for (auto & chromosome : new_population) {
            chromosome = mutation(chromosome,p_mutation);
        }
        population = new_population;
        population_fit = fitness(population);
    }
    return population;
};


std::vector<double> fintess_function(population_t pop){
    // chromosom binarnie => całości + ułamek f decode
    double x = pop.at(0); // część całości
    double y = pop.at(1); // część ułamkowa
    cos = pow(x * x + y - 11, 2) + pow(x + y * y - 7, 2);
}
std::vector<int> selection_empty(std::vector<double> fitnesses) {
    return {};
}
std::vector<chromosome_t > crossover_empty(std::vector<chromosome_t > parents) {
    return parents;
}
chromosome_t mutation_empty(chromosome_t parents, double p_mutation) {
    return parents;
}


int main() {
    using namespace std;
    population_t population = {{1,0,1,0,1,0,1}, {1,0,1,0,1,0,1}};
    auto result = genetic_algorithm(population,
                                    fintess_function,
                                    [](auto a, auto b){return true;},
                                    selection_empty, 1.0,
                                    crossover_empty,
                                    0.01, mutation_empty);
    for (chromosome_t chromosome: result) {
        cout << "[";
        for (int p: chromosome) {
            cout << p;
        }
        cout << "] ";
    }
    cout << endl;
    return 0;
}
