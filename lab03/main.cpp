#include <math.h>
#include <iostream>
#include <vector>
#include <functional>
#include <random>

std::random_device rd;
std::mt19937 mt_generator(rd());

std::vector<double> full_review(std::function<double(std::vector<double>)> f, std::vector<double>point, std::vector<double>domain, int iterations){
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    vector<double> min = {dist(mt_generator), dist(mt_generator)};
    double result = f(min);
    for (int i = 0; i < iterations; i++){
        vector<double> args = {dist(mt_generator), dist(mt_generator)};
        double comp = f(args);
        if (comp < result){
            result = comp;
            min = args;
        }
    }
    return min;
}

std::vector<double> get_neighbour(std::vector<double> ar) {
    double a = ar.at(0);
    double b = ar.at(1);

    std::uniform_real_distribution<double> step(-0.004, 0.004);

    a += step(mt_generator);
    b += step(mt_generator);

    return {a, b};
}

std::vector<double> hill_climbing(std::function<double(std::vector<double>)> f, std::vector<double> point, std::vector<double>domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    vector<double> min = point;

    double result = f(min);
    for (int i = 0; i < iterations; i++) {
        std::vector<double> args = get_neighbour(min);
        if (args[0] > point.at(1) or args.at(0) < point.at(0) or args.at(1) > point.at(1) or
            args.at(1) < point.at(0)) {
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

std::vector<double> simulated_annealing(std::function<double(std::vector<double>)> f, std::vector<double> point, std::vector<double>domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));

    vector<double> min = point;
    vector<double> args = {dist(mt_generator), dist(mt_generator)};

    double result = f(min);
    vector<vector<double>> visited = {};

    for (int i = 0; i < iterations; i++) {
        vector<double> neighbour = get_neighbour(min);
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
    vector<double> point = {-1, 1};
    int iterations = 10000;

    std::vector<double> boothResult = full_review(booth, point, domain, iterations);
    std::vector<double> matyasResult = full_review(matyas, point, domain, iterations);
    std::vector<double> himmelblauResult = full_review(himmelblau, domain, point, iterations);

    cout << endl << "Full review:" << endl;
    cout << "booth: " << boothResult.at(0) << " " << boothResult.at(1) << endl;
    cout << "matyas: " << matyasResult.at(0) << " " << matyasResult.at(1) << endl;
    cout << "himmelblau: " << himmelblauResult.at(0) << " " << himmelblauResult.at(1) << endl;

    boothResult = simulated_annealing(booth, point, domain, iterations);
    matyasResult = simulated_annealing(matyas, point, domain, iterations);
    himmelblauResult = simulated_annealing(himmelblau, domain, point, iterations);

    cout << endl << "Simmulated annealing" << endl;
    cout << "booth: " << boothResult.at(0) << " " << boothResult.at(1) << endl;
    cout << "matyas: " << matyasResult.at(0) << " " << matyasResult.at(1) << endl;
    cout << "himmelblau: " << himmelblauResult.at(0) << " " << himmelblauResult.at(1) << endl;

    boothResult = hill_climbing(booth, point, domain, iterations);
    matyasResult = hill_climbing(matyas, point, domain, iterations);
    himmelblauResult = hill_climbing(himmelblau, domain, point, iterations);

    cout << endl << "Hill Climbing:" << endl;
    cout << "booth: " << boothResult.at(0) << " " << boothResult.at(1) << endl;
    cout << "matyas: " << matyasResult.at(0) << " " << matyasResult.at(1) << endl;
    cout << "himmelblau: " << himmelblauResult.at(0) << " " << himmelblauResult.at(1) << endl;

    return 0;
}
