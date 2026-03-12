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

int main (int argc, char *argv[]){

    // Set random number generator seed
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

    double L = 1.;
    double d = 5.;
    int M = 100000;
    int N = 100;
    int m = M/N;
    std::vector<double> averages(N), averages2(N);
    std::vector<double> cumsum_pi(N), cumsum_pi2(N), errors_pi(N);

    for (int i = 0; i < N; i++) {
        int N_hit = 0;
        for (int j = 0; j < m; j++) {
            double needle_start = rnd.Rannyu(0., d);
            double x_rand, y_rand, distance;
            do {
                x_rand = rnd.Rannyu(-L, L);
                y_rand = rnd.Rannyu(-L, L);
                distance = std::pow(x_rand, 2) +  std::pow(y_rand, 2);
            } while (distance > L*L);
            
            double needle_end = needle_start + y_rand * L / std::sqrt(distance);
            if (needle_end <= 0. || needle_end >= d) N_hit++;
        }

        averages[i] = 2*L*static_cast<double>(m)/(d*static_cast<double>(N_hit));
        averages2[i] = averages[i] * averages[i];
    }

    cumsum_pi = Cumsum(averages);
    cumsum_pi2 = Cumsum(averages2);

    for (int i = 0; i < N; i++) {
        cumsum_pi[i] /= static_cast<double>(i + 1);
        cumsum_pi2[i] /= static_cast<double>(i + 1);
        if (i == 0) {
            errors_pi[i] = 0.;
            continue;
        }
        errors_pi[i] = std::sqrt((cumsum_pi2[i] - std::pow(cumsum_pi[i], 2)) / static_cast<double>(i));
    }

    // Write results to file
    std::ofstream out;
    out.open("pi.out");
    if (out.is_open()) {
        for (int i = 0; i < N; i++) {
            out << cumsum_pi[i] << " " << errors_pi[i] << std::endl;
        }
    } else std::cerr << "Error: unable to write pi.out" << std::endl;
    out.close();

    return 0;
}