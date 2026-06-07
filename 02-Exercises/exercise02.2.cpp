#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "random.h"
#include "random_walk.h"

using namespace std;

int main (int argc, char *argv[]){

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");

    int M = 10000;
    int N = 50;
    int L = M/N;
    int max_steps = 100;
    vector<double> quadratic_distances(max_steps), quadratic_distances_errors(max_steps);
    vector<vector<double>> positions_lattice(M, vector<double>{0.,0.,0.});

    // Execute max_steps steps on M cubic lattice random walks
    for (int i = 0; i < max_steps; i++) {
        vector<double> datablocks(N), datablocks2(N);
        
        double sum_datablocks = 0.;
        double sum_datablocks2 = 0.;

        for (int j = 0; j < N; j++) {
            double sum = 0.;
            // Advance one step in every RW and add its distance to the accumulator
            for (int k = 0; k < L; k++) {
                int index = L*j+k;
                positions_lattice[index] = RW_step_lattice(positions_lattice[index], rnd);
                sum += compute_distance(positions_lattice[index]);
            }

            sum_datablocks += sqrt(sum / static_cast<double>(L));
            sum_datablocks2 += sum / static_cast<double>(L);
        }

        sum_datablocks /= static_cast<double>(N);
        sum_datablocks2 /= static_cast<double>(N);
        // Calculate sqrt(<r^2>) and its error with data blocking
        quadratic_distances[i] = sum_datablocks;
        quadratic_distances_errors[i] = sqrt((sum_datablocks2 - pow(sum_datablocks, 2))/static_cast<double>(N-1));
    }
    
    ofstream outfile;

    // Write results to file
    outfile.open("OUTPUT/RW_lattice.out");
    if (outfile.is_open()) {
        for (int i = 0; i < max_steps; i++) {
            outfile << i+1 << " " << quadratic_distances[i] << " " << quadratic_distances_errors[i] << endl;
        }
    } else cerr << "Error: unable to write RW_lattice.out" << endl;
    outfile.close();

    vector<vector<double>> positions_continuum(M, vector<double>{0.,0.,0.});

    for (int i = 0; i < max_steps; i++) {
        vector<double> datablocks(N), datablocks2(N);
        
        double sum_datablocks = 0.;
        double sum_datablocks2 = 0.;

        for (int j = 0; j < N; j++) {
            double sum = 0.;
            // Advance one step in every RW and add its distance to the accumulator
            for (int k = 0; k < L; k++) {
                int index = L*j+k;
                positions_continuum[index] = RW_step_continuum(positions_continuum[index], rnd);
                sum += compute_distance(positions_continuum[index]);
            }

            sum_datablocks += sqrt(sum / static_cast<double>(L));
            sum_datablocks2 += sum / static_cast<double>(L);
        }

        sum_datablocks /= static_cast<double>(N);
        sum_datablocks2 /= static_cast<double>(N);
        // Calculate sqrt(<r^2>) and its error with data blocking
        quadratic_distances[i] = sum_datablocks;
        quadratic_distances_errors[i] = sqrt((sum_datablocks2 - pow(sum_datablocks, 2))/static_cast<double>(N-1));
    }

    // Write results to file
    outfile.open("OUTPUT/RW_continuum.out");
    if (outfile.is_open()) {
        for (int i = 0; i < max_steps; i++) {
            outfile << i+1 << " " << quadratic_distances[i] << " " << quadratic_distances_errors[i] << endl;
        }
    } else cerr << "Error: unable to write RW_continuum.out" << endl;
    outfile.close();

   return 0;
}