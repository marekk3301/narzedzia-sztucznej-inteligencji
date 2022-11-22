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
    double P1=0;
    double pi;
    double pi1=0;
    std::vector<int> vec;
    std::uniform_real_distribution<> rnd(0,1);
    double R = rnd(mt_generator);

    for(double e : fitnesses){
        S+=e;
    }
    for (int i = 0; i < fitnesses.size(); i++) {
        pi = fitnesses.at(i) / S;
        P = P1 + pi;
        if(P1 <= R && pi1 <= P){
            vec.push_back(i);
        }
        P1 = P;
    }

    return vec;
}

std::vector<chromosome_t > crossover(std::vector<chromosome_t > parents) {
    std::uniform_real_distribution<double> S(0,genotype_len); //Todo genotype_len
    int rand = S(mt_generator);
    for(int i = rand; i < parents.at(0).size();i++) {
        int index = parents.at(0).at(i);
        parents.at(0).at(i) = parents.at(1).at(i);
        parents.at(1).at(i) = index;
    }

    return parents;
}

chromosome_t mutation(chromosome_t parents, double p_mutation) {
    std::uniform_real_distribution<> randNum(0,1);
    std::uniform_real_distribution<> randQuan(0,8);
    std::uniform_real_distribution<> randPoint(0,parents.size());
    int Quantity = randQuan(mt_generator);
    if(randNum(mt_generator)<p_mutation){
        for(int i = Quantity ; i>0 ; i--){
            int temp = randPoint(mt_generator);
            parents.at(temp) = randNum(mt_generator);
        }
    }

    return parents;
}

int main() {
    using namespace std;
    cout << "genotype_len: " << genotype_len << endl;

//    population_t population = {{1,0,1,0,1,0,1}, {1,0,1,0,1,0,1}};
    population_t population = generate_population(100, genotype_len);

    try {
        auto result = genetic_algorithm(population,
                                        fitness_function,
                                        [](auto a, auto b) {
                                            for (auto count: b) {
                                                if (count > 997) {
                                                    
                                                    return true;
                                                }
                                            }

                                            return false;
                                        },
                                        selection, 1.0,
                                        crossover,
                                        0.01, mutation);
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
