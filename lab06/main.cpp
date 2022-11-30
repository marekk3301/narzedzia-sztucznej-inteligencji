#include <iostream>
#include <vector>
#include <functional>
#include <random>

std::random_device rd;
std::mt19937 mt_generator(rd());

using chromosome_t = std::vector<int>;
using population_t = std::vector<chromosome_t>;


// 100 + ('2262')7*2
int genotype_len = 100 + (22627 % 10) * 2; // 114

double calculate_min_fitness(std::vector<double> fitnesses) {
    double min = fitnesses.at(0);
    for (double fitness : fitnesses) {
        if (fitness < min) {
            min = fitness;
        }
    }

    return min;
}

double calculate_max_fitness(std::vector<double> fitnesses) {
    double max = fitnesses.at(0);
    for (double fitness : fitnesses) {
        if (fitness > max) {
            max = fitness;
        }
    }

    return max;
}

double calculate_avg_fitness(std::vector<double> fitnesses) {
    double avg = 0;

    for (double fitness : fitnesses) {
        avg += fitness;
    }
    avg /= fitnesses.size();

    return avg;
}

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
//        cout << "im doing genetics!" << endl;
        auto parents_indexes = selection(population_fit);
        decltype(population) new_population;
        for (int i = 0 ; i < parents_indexes.size(); i+=2) {
            decltype(initial_population) offspring = {population[i],population[i+1]};
            if (uniform(mt_generator) < p_crossover) {
                offspring = crossover(offspring);
            }
            for (const chromosome_t& chromosome : offspring) new_population.push_back(chromosome);
        }
        for (auto & chromosome : new_population) {
            chromosome = mutation(chromosome,p_mutation);
        }
        population = new_population;
        population_fit = fitness(population);
    }

    return population;
};

auto himmelblau = [](std::vector<double> args) {
    double x = args.at(0);
    double y = args.at(1);

    return pow(x * x + y - 11, 2) + pow(x + y * y - 7, 2);
};

std::vector<double> decode(chromosome_t chromosome) {
    using namespace std;
    double x = 0.0;
    double y = 0.0;

    for (int i = 1; i < genotype_len / 2; i++) {
        x += chromosome[i] * (1 / pow(2, i));
    }

    if (chromosome[0] == 1) {
        x *= -1.0;
    }

    for (int i = genotype_len / 2 + 1; i < genotype_len; i++) {
        y += chromosome[i] * (1 / pow(2, i - (genotype_len / 2)));
    }

    if (chromosome[genotype_len / 2] == 1) {
        y *= -1.0;
    }

    vector<double> result = {5 * x, 5 * y};

    return result;
}

std::vector<double> fitness_function(const population_t& pop){
    using namespace std;
    vector<double> fitnesses;

    for (const chromosome_t& chromosome : pop) {
        auto fitness = 1000 - himmelblau(decode(chromosome));
        if (fitness < 0) { fitness = 0; }
        fitnesses.push_back(fitness);
//        cout << fitness << endl;
    }

    return fitnesses;
}

std::vector<int> selection(std::vector<double> fitnesses) {
    using namespace std;
//    https://stackoverflow.com/questions/10531565/how-should-roulette-wheel-selection-be-organized-for-non-sorted-population-in-g
//    To perform roulette selection, you need only pick a random number in the range 0 to 240 (the sum of the population
//    's fitness). Then, starting at the first element in the list, subtract each individual's fitness until the random number is less than or equal to zero.

//    while R > 0 do:
//    r = r - fitness of individual #i
//    increment i
//    select individual #i - 1 for reproduction.

    double S=0;
    double P;
    double P0=0;
    double pi;
    double p0=0;
    std::vector<int> vec;
    std::uniform_real_distribution<> rnd(0,1);
    double R = rnd(mt_generator);

    for(double e : fitnesses){
        S+=e;
    }
    for (int i = 0; i < fitnesses.size(); i++) {
        pi = fitnesses.at(i) / S;
        P = P0 + pi;
        if(P0 <= R && p0 <= P){
            vec.push_back(i);
        }
        P0 = P;
    }

    return vec;
}

std::vector<chromosome_t > crossover(std::vector<chromosome_t > parents) {
    std::uniform_real_distribution<double> S(0,genotype_len);
    int rand = S(mt_generator);
    for(int i = rand; i < genotype_len; i++) {
        int index = parents.at(0).at(i);
        parents.at(0).at(i) = parents.at(1).at(i);
        parents.at(1).at(i) = index;
    }

    return parents;
}

chromosome_t mutation(chromosome_t parents, double p_mutation) {
    std::uniform_real_distribution<> randNum(0,1);
    std::uniform_real_distribution<> randQuan(0,8);
    std::uniform_real_distribution<> randPoint(0,genotype_len);
    int Quantity = randQuan(mt_generator);
        for (int i = Quantity ; i > 0 ; i--) {
            if (randNum(mt_generator) < p_mutation) {
                int temp = randPoint(mt_generator);
                parents.at(temp) = randNum(mt_generator);
            }
        }

    return parents;
}

float calculateSD(std::vector<double> population_fit) {
    double sum = 0.0;
    double mean;
    double standardDeviation = 0.0;

    for(int i = 0; i < 10; ++i) {
        sum += population_fit.at(i);
    }

    mean = sum / 10;

    for(int i = 0; i < 10; ++i) {
        standardDeviation += pow(population_fit.at(i) - mean, 2);
    }

    return sqrt(standardDeviation / 10);
}

int main() {
    using namespace std;

    bool standard_deviation = true;
    int iterations = 0;
    double sd_threshold = 100;

    int s_population = 100;
    int max_iterations = 100;
    double p_crossover = 1.0;
    double p_mutation = 0.01;

//    cout << "Population size: ";
//    cin >> s_population;
//    cout << "Iteration number: ";
//    cin >> max_iterations;
//    cout << "Crossover probability: ";
//    cin >> p_crossover;
//    cout << "Mutation probability: ";
//    cin >> p_mutation;
//    cout << "genotype_len: " << genotype_len << endl;

//    population_t population = {{1,0,1,0,1,0,1}, {1,0,1,0,1,0,1}};
    population_t population = generate_population(s_population, genotype_len);

    try {
        auto result = genetic_algorithm(population,
                                        fitness_function,
                                        [&standard_deviation, &iterations, &max_iterations, &sd_threshold](auto population, auto population_fit) {
                                            if (standard_deviation) {
                                                cout << "Fitness standard deviation: " << calculateSD(population_fit) << endl;
                                                if (calculateSD(population_fit) < sd_threshold) {
                                                    return true;
                                                } else {return false;}
                                            } //else {
                                            if (iterations < max_iterations) {

                                                return false;
                                            }
                                            // }

                                            return true;
                                        },
                                        selection, p_crossover,
                                        crossover,
                                        p_mutation, mutation);
        for (const chromosome_t& chromosome: result) {
            cout << "[";
            for (int p: chromosome) {
                cout << p;
            }
            cout << "] " << endl;
        }
    } catch(out_of_range& aor) {

        return 1;
    }

    cout << endl;

    return 0;
}
