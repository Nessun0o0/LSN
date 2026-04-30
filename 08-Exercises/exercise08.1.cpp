#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "variationalMC.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");
    
    double x, sigma, mu, delta;
    ifstream input("input_1.dat");
    if (input.is_open()) {
        input >> x >> mu >> sigma >> delta;
    } else {
        cerr << "ERROR: can't open input_1.dat" << endl;
        exit(-1);
    }
    input.close();

    ExpectationEnergy H_var(x, mu, sigma, delta, &rnd);

    int N = 100, L=1000;
    vector<double> sum_H(N), sum2_H(N), error_H(N);

    for (int i = 0; i < N; i++) {
        double block_sum = 0.;
        for (int j = 0; j < L; j++) {
            H_var.metro();
            block_sum += H_var.H_psi_T();
        }
        sum_H[i] = block_sum / static_cast<double>(L);
        sum2_H[i] = sum_H[i]*sum_H[i];
    }

    error_H[0] = 0.;
    for (int i = 1; i < N; i++) {
        sum_H[i] += sum_H[i-1];
        sum2_H[i] += sum2_H[i-1];
        error_H[i] = sqrt((sum2_H[i]/(i+1) - pow(sum_H[i]/(i+1), 2))/i);
    }
    cout << sum_H[N-1] / N << " +- " << error_H[N-1] << endl;

    cout << H_var.get_acceptance() << endl;

    ofstream output("output_1.out");
    if (output.is_open()) {
        for (int i = 0; i < N; i++) {
            output << i+1 << "\t" << sum_H[i] / (i+1) << "\t" << error_H[i] << endl;
        }
    } else {
        cerr << "ERROR: can't open output_1.dat" << endl;
    }
    output.close();

    return 0;
}