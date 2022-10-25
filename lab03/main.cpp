#include <math.h>
#include <iostream>
#include <vector>
#include <functional>
#include <random>

std::random_device rd;
std::mt19937 mt_generator(rd());

std::vector<double> bruteforce(auto f, std::vector<double> domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    vector<double> min = f(domain);

    double result = f(min);
    for (int i = 0; i < iterations - 1; i++) {
        vector<double> args = {dist(mt_generator), dist(mt_generator)};
        double comp = f(args);
        if (comp < result) {
            result = comp;
            min = args;
        }
    }
    return min;
}

std::vector<double> getNeighbour(std::vector<double> ar) {
    double a = ar.at(0);
    double b = ar.at(1);

    std::uniform_real_distribution<double> step(-0.004, 0.004);

    a += step(mt_generator);
    b += step(mt_generator);

    return {a, b};
}

std::vector<double> hill_climbing(auto f, std::vector<double> domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    vector<double> min = f(domain.at(0), domain.at(1));

    double result = f(min);
    for (int i = 0; i < iterations; i++) {
        std::vector<double> args = getNeighbour(min);
        if (args[0] > domain.at(1) or args.at(0) < domain.at(0) or args.at(1) > domain.at(1) or
            args.at(1) < domain.at(0)) {
            continue;
        }
        double comp = f(args);
        if (comp < result) {
            result = comp;
            min = args;
        }
    }
    return min;
}

std::vector<double> simulated_annealing(auto f, std::vector<double> domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));

    vector<double> min = f(domain.at(0), domain.at(1));
    vector<double> args = f(dist(mt_generator), dist(mt_generator));

    double result = f(min);
    vector<vector<double>> visited = {};

    for (int i = 0; i < iterations; i++) {
        vector<double> neighbour = getNeighbour(min);
        double comp = f(neighbour);

        if (comp < result) {
            result = comp;
            min = neighbour;
        } else {
            std::uniform_real_distribution<double> rand(0, 1);
            double temp = comp - result;
            if (temp < 0) {
                temp = temp * -1;
            }
            double Ti = 10000.0 / i;
            if (rand(mt_generator) < exp(-1 * temp) / Ti) {
                min = neighbour;
            }
        }
    }
    return min;
}

int main() {
    using namespace std;

    auto booth = [](vector<double> args) {
        double x = args.at(0);
        double y = args.at(1);
        return pow(x + 2 * y - 7, 2) + pow(2 * x + y - 5, 2);
    };

    auto matyas = [](vector<double> args) {
        double x = args.at(0);
        double y = args.at(1);
        return 0.26 * (x * x + y * y) - 0.48 * x * y;
    };

    auto himmelblau = [](vector<double> args) {
        double x = args.at(0);
        double y = args.at(1);
        return pow(x * x + y - 11, 2) + pow(x + y * y - 7, 2);
    };

    vector<double> domain = {-4.5, 4.5};
    int iterations = 10000;

    std::vector<double> boothResult = bruteforce(booth, domain, iterations);
    std::vector<double> matyasResult = bruteforce(booth, domain, iterations);
    std::vector<double> himmelblauResult = bruteforce(booth, domain, iterations);

//    std::vector<double> boothResult = hill_climbing(booth, domain, iterations);
//    std::vector<double> matyasResult = hill_climbing(booth, domain, iterations);
//    std::vector<double> himmelblauResult = hill_climbing(booth, domain, iterations);

//    std::vector<double> boothResult = simulated_annealing(booth, domain, iterations);
//    std::vector<double> matyasResult = simulated_annealing(booth, domain, iterations);
//    std::vector<double> himmelblauResult = simulated_annealing(booth, domain, iterations);

    cout << "booth: " << boothResult.at(0) << " " << boothResult.at(1) << endl;
    cout << "matyas: " << matyasResult.at(0) << " " << matyasResult.at(1) << endl;
    cout << "himmelblau: " << himmelblauResult.at(0) << " " << himmelblauResult.at(1) << endl;

    return 0;
}
