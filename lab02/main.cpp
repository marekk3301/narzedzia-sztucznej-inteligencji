#include <math.h>
#include <iostream>
#include <vector>
#include <functional>
#include <random>

std::random_device rd;
std::mt19937 mt_generator(rd());

double findMin(auto f, std::vector<double> domain, int iterations) {
    using namespace std;
    uniform_real_distribution<double> dist(domain.at(0), domain.at(1));
    double min = f(domain.at(0), domain.at(1));
    for(int i=0;i<iterations-1;i++){
        double rand1=dist(mt_generator);
        double rand2=dist(mt_generator);
        double comp;
        comp = f(rand1,rand2);
        if(comp < min) {
            min=comp;
        }
    }
    return min;
}

int main() {
    using namespace std;
    double result = 0;

    auto booth = [](double x, double y) {
        return pow(x + 2*y - 7, 2) + pow(2*x + y - 5, 2);
    };

    auto matyas = [](double x, double y) {
        return 0.26 * (x*x + y*y) - 0.48*x*y;
    };

    auto himmelblau = [](double x, double y) {
        return pow(x*x + y - 11, 2) + pow(x + y*y - 7, 2);
    };

    vector<double> domain = {-4.5, 4.5};
    int iterations = 1000000;
    int expLength = 20;
    double avgBooth, avgMatyas, avgHimmelblau;
    for (int i=0; i<expLength; i++) {
        avgBooth += findMin(booth,domain,iterations);
        avgMatyas += findMin(matyas,domain,iterations);
        avgHimmelblau += findMin(himmelblau,domain,iterations);
    }
    cout << "booth: " << avgBooth / expLength << endl;
    cout << "matyas: " << avgMatyas / expLength << endl;
    cout << "himmelblau: " << avgHimmelblau / expLength << endl;

    return 0;
}
