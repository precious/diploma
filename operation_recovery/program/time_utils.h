#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include <functional>
using namespace std;

typedef float real;

#define RAND(n) rand()%n

extern int CLOCK_ID;

typedef uniform_real_distribution<double> UniformDistribution;
typedef normal_distribution<double> GaussianDistribution;

typedef mt19937 Engine;


template <typename Eng,typename Distrib>
class Generator {
private:
    Eng engine;
    Distrib distribution;
public:
    Generator(Eng e,Distrib d): engine(e), distribution(d) {}
    double operator() () {
        return distribution(engine);
    }
};

typedef Generator<Engine,UniformDistribution> UniformDistributionGenerator;
typedef Generator<Engine,GaussianDistribution> GaussianDistributionGenerator;

void printTimespec(timespec*);

UniformDistributionGenerator* getUniformDistributionGenerator(double,double);
GaussianDistributionGenerator* getGaussianDistributionGenerator(double,double);

timespec* getTimespecDelta(timespec*,timespec*);

double getRandom();



#endif // TIME_UTILS_H

