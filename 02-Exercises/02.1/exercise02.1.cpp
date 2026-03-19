#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "random.h"

std::vector<double> Cumsum(const std::vector<double>& vec) {
    std::vector<double> cumsum_vec(vec.size());

    cumsum_vec[0] = vec[0];
    for (int i = 1; i < vec.size(); i++) {
        cumsum_vec[i] = cumsum_vec[i-1] + vec[i];
    }

    return cumsum_vec;
}

double f(double x) {
    return std::cos(M_PI*x/2.)*M_PI/2.;
}

int main (int argc, char *argv[]){

    Random rnd;
    int seed[4];
    int p1, p2;
    std::ifstream Primes("Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else { 
        std::cerr << "PROBLEM: Unable to open Primes" << std::endl;
        exit(-1);
    }
    Primes.close();

    std::ifstream input("seed.in");
    std::string property;
    if (input.is_open()){
        while ( !input.eof() ){
            input >> property;
            if( property == "RANDOMSEED" ){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed,p1,p2);
            }
        }
        input.close();
    } else {
        std::cerr << "PROBLEM: Unable to open seed.in" << std::endl;
        exit(-1);
    }

    int M = 10000;
    int N = 100;
    int L = M/N;
    std::vector<double> uniform(N), uniform2(N);
    std::vector<double> cumsum_uniform(N), cumsum_uniform2(N), errors_uniform(N);

    for (int i = 0; i < N; i++) {
        double sum = 0.;
        for (int j = 0; j < L; j++) {
            sum += f(rnd.Rannyu());
        }
        uniform[i] = sum / static_cast<double>(L);
        uniform2[i] = uniform[i] * uniform[i];
    }

    cumsum_uniform = Cumsum(uniform);
    cumsum_uniform2 = Cumsum(uniform2);

    for (int i = 0; i < N; i++) {
        cumsum_uniform[i] /= static_cast<double>(i + 1);
        cumsum_uniform2[i] /= static_cast<double>(i + 1);
        if (i == 0) {
            errors_uniform[i] = 0.;
            continue;
        }
        errors_uniform[i] = std::sqrt((cumsum_uniform2[i] - std::pow(cumsum_uniform[i], 2)) / static_cast<double>(i));
    }

    std::vector<double> importance(N), importance2(N);
    std::vector<double> cumsum_importance(N), cumsum_importance2(N), errors_importance(N);

    for (int i = 0; i < N; i++) {
        double sum = 0.;
        for (int j = 0; j < L; j++) {
            double x = 1-std::sqrt(1.-rnd.Rannyu());
            sum += std::cos(M_PI*x/2.)*M_PI/4./(1.-x);
        }
        importance[i] = sum / static_cast<double>(L);
        importance2[i] = importance[i] * importance[i];
    }

    cumsum_importance = Cumsum(importance);
    cumsum_importance2 = Cumsum(importance2);

    for (int i = 0; i < N; i++) {
        cumsum_importance[i] /= static_cast<double>(i + 1);
        cumsum_importance2[i] /= static_cast<double>(i + 1);
        if (i == 0) {
            errors_importance[i] = 0.;
            continue;
        }
        errors_importance[i] = std::sqrt((cumsum_importance2[i] - std::pow(cumsum_importance[i], 2)) / static_cast<double>(i));
    }

    std::ofstream outfile;

    outfile.open("uniform.out");
    if (outfile.is_open()) {
        for (int i = 0; i < N; i++) {
            outfile << N*i << " " << cumsum_uniform[i] << " " << errors_uniform[i] << std::endl;
        }
    } else std::cerr << "Error: unable to write uniform.out" << std::endl;
    outfile.close();

    outfile.open("importance.out");
    if (outfile.is_open()) {
        for (int i = 0; i < N; i++) {
            outfile << N*i << " " << cumsum_importance[i] << " " << errors_importance[i] << std::endl;
        }
    } else std::cerr << "Error: unable to write importance.out" << std::endl;
    outfile.close();

   return 0;
}