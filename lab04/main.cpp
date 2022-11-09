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


auto generate_population = [](int population_size, int genotype_len) {
    using namespace std;
    population_t population;
    uniform_int_distribution<int> rand(0, 1);

    for (int i = 0; i < population_size; i++) {
        chromosome_t chromosome;
        for (int j = 0; j < genotype_len; j++) {
            chromosome.push_back(rand(mt_generator));
        }
        population.push_back(chromosome);
    }

    return population;
};

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

auto booth = [](std::vector<double> args) {
    double x = args.at(0);
    double y = args.at(1);
    return pow(x + 2 * y - 7, 2) + pow(2 * x + y - 5, 2);
};

double findMax(auto f, std::vector<double> domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    double max = f(domain.at(0), domain.at(1));
    for(int i=0;i<iterations-1;i++) {
        double rand1=dist(mt_generator);
        double rand2=dist(mt_generator);
        double comp;
        comp = f(rand1,rand2);
        if(comp > max) {
            max = comp;
        }
    }
    return max;
}

std::vector<double> decode(chromosome_t chromosome) {
    using namespace std;

    double x = 0.0;
    double y = 0.0;

    int genotype_len = chromosome.size();

    for (int i = 0; i < genotype_len / 2; i++) {
        x *= 2;
        x += chromosome[i];
    }

    for (int i = genotype_len / 2; i < genotype_len; i++) {
        y *= 2;
        y += chromosome[i];
    }

    x = x / pow(2.0, (chromosome.size() / 2 - 4)) - 8;
    y = y / pow(2.0, (chromosome.size() / 2 - 4)) - 8;

    vector<double> result = {x, y};
    return result;
}

std::vector<double> fitness_function(population_t pop){
    using namespace std;
    vector<double> fitnesses;

//        cos = pow(x * x + y - 11, 2) + pow(x + y * y - 7, 2);

    for (chromosome_t chromosome : pop) {
        auto fitness = booth(decode(chromosome));
        fitnesses.push_back(fitness);
    }

    return fitnesses;
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

    // 100 + ('2262')7*2
    int genotype_len = 100 + (22627 % 10) * 2; // 114
    cout << "genotype_len: " << genotype_len << endl;


//    population_t population = {{1,0,1,0,1,0,1}, {1,0,1,0,1,0,1}};
    population_t population = generate_population(20, genotype_len);

    auto result = genetic_algorithm(population,
                                    fitness_function,
                                    [](auto a, auto b){return true;},
                                    selection_empty, 1.0,
                                    crossover_empty,
                                    0.01, mutation_empty);
    for (chromosome_t chromosome: result) {
        cout << "[";
        for (int p: chromosome) {
            cout << p;
        }
        cout << "] " << endl;
    }

//    for (chromosome_t chromosome: result) {
//        cout << "[";
//        for (double p: decode(chromosome)) {
//            cout << p << " ";
//        }
//        cout << "] " << endl;
//    }
    cout << endl;
    return 0;
}
