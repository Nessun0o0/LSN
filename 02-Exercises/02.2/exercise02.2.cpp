#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "random.h"
#include "random_walk.h"

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
    int max_steps = 100;
    std::vector<double> quadratic_distances(max_steps), quadratic_distances_errors(max_steps);
    std::vector<std::vector<double>> positions(M, std::vector<double>{0.,0.,0.});

    for (int i = 0; i < max_steps; i++) {
        std::vector<double> datablocks(N), datablocks2(N);
        
        double sum_datablocks = 0.;
        double sum_datablocks2 = 0.;

        for (int j = 0; j < M; j++) {
            for (auto RW : positions) {
                RW = RW_step_lattice(RW, rnd);
            }
        }
        for (int j = 0; j < N; j++) {
            double sum = 0.;
            for (int k = 0; k < L; k++) {
                sum += compute_distance(positions[N*j+k]);
            }

            //datablocks2[j] = sum / static_cast<double>(L);
            //datablocks[j] = std::sqrt(datablocks2[j]);
            sum_datablocks2 += sum / static_cast<double>(L);
            sum_datablocks += std::sqrt(sum / static_cast<double>(L));
        }

        
        /*for (int j = 0; j < N; j++) {
            sum_datablocks += datablocks[j];
            sum_datablocks2 += datablocks2[j];
        }*/
        sum_datablocks /= static_cast<double>(N);
        sum_datablocks2 /= static_cast<double>(N);
        quadratic_distances[i] = sum_datablocks;
        quadratic_distances_errors[i] = std::sqrt((sum_datablocks2 - std::pow(sum_datablocks, 2))/static_cast<double>(N-1));
        //std::cout << sum_datablocks << " " << sum_datablocks2 << " " << std::sqrt((sum_datablocks2 - std::pow(sum_datablocks, 2))/static_cast<double>(N-1)) << std::endl;
        //std::cout << "\n";
    }
    
    std::ofstream outfile;

    outfile.open("RW_lattice.out");
    if (outfile.is_open()) {
        for (int i = 0; i < max_steps; i++) {
            outfile << i << " " << quadratic_distances[i] << " " << quadratic_distances_errors[i] << std::endl;
        }
    } else std::cerr << "Error: unable to write RW_lattice.out" << std::endl;
    outfile.close();

   return 0;
}