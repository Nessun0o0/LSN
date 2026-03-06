#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "random.h"

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

    // Declare common variables
    int N[] = {1, 2, 10, 100};
    int M = 10000;

    
    for (int n: N) {
        std::vector<double> averages_standard(M), averages_exp(M), averages_lorentz(M);
        
        // Standard dice
        for (int i = 0; i < M; i++) {
            double sum = 0.;
            for (int j = 0; j < n; j++) {
                sum += rnd.Rannyu();
            }
            averages_standard[i] = sum / static_cast<double>(n);
        }

        // Exponential dice
        for (int i = 0; i < M; i++) {
            double sum = 0.;
            for (int j = 0; j < n; j++) {
                sum += rnd.Exp(1.);
            }
            averages_exp[i] = sum / static_cast<double>(n);
        }

        // Lorentian dice
        for (int i = 0; i < M; i++) {
            double sum = 0.;
            for (int j = 0; j < n; j++) {
                sum += rnd.Lorentz(0., 1.);
            }
            averages_lorentz[i] = sum / static_cast<double>(n);
        }

        // Write results to file
        std::ofstream out;
        std::string filename = std::to_string(n) + ".out";
        out.open(filename);
        if(out.is_open()) {
            for (int i = 0; i < M; i++) {
                out << averages_standard[i] << " " << averages_exp[i] << " " << averages_lorentz[i] << std::endl;
            }
        } else std::cerr << "Error: unable to write " + filename << std::endl;

    }

   return 0;
}