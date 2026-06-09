#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "variationalMC.h"
#include "data_blocking.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");
    
    // Input
    double x, sigma, mu, delta;
    ifstream input("input_1.dat");
    if (input.is_open()) {
        char tmp[512];
        for (int i = 0; i < 5; i++) input >> tmp;
        input >> x >> mu >> sigma >> delta;
    } else {
        cerr << "ERROR: can't open input_1.dat" << endl;
        exit(-1);
    }
    input.close();

    ExpectationEnergy H_var(x, mu, sigma, delta, &rnd);

    int N = 100, L=1000;
    vector<double> sum_H(N), sum2_H(N), error_H(N);

    // Performs metropolis steps and saves the results inside the data blocks
    for (int i = 0; i < N; i++) {
        double block_sum = 0.;
        for (int j = 0; j < L; j++) {
            H_var.metro();
            block_sum += H_var.H_psi_T();
        }
        sum_H[i] = block_sum / static_cast<double>(L);
        sum2_H[i] = sum_H[i]*sum_H[i];
    }

    // Compute averages and errors and write results to file
    averages_from_blocks(sum_H, sum2_H, error_H);
    write_data(sum_H, error_H, "OUTPUT/output_1.out");

    return 0;
}