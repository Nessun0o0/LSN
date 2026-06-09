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
    
    // Input parameters from file
    double mu, sigma, delta_SA, T, cooling_rate;
    int n_sim;
    ifstream input("input_2.dat");
    if (input.is_open()) {
        char tmp[512];
        for (int i = 0; i < 7; i++) input >> tmp;
        input >> mu >> sigma >> delta_SA >> T >> cooling_rate >> n_sim;
    } else {
        cerr << "ERROR: can't open input_1.dat" << endl;
        exit(-1);
    }
    input.close();

    int n_temps = ceil(log(1e-3/T)/log(cooling_rate)); // Number of temperatures explored by the algorithm
    int step = 1;

    SimulatedAnnealing SA(mu, sigma, T, delta_SA, &rnd);
    SA.start_annealing();

    while (T > 1e-3) {
        // Perform the SA step with n_sim Metropolis moves
        SA.annealing_step(n_sim);
        
        cout << "Completed temperature " << step << "/" << n_temps << endl;

        // Decreases temperature with an exponential decay
        T *= cooling_rate;
        SA.set_temp(T);
        // Increases number of steps to have a smaller error for smaller temperatures
        SA.set_compute_H_steps(10000 + 100/T);
        step++;
    }

    // Computes the expectation energy with the optimized parameters with data blocking
    ExpectationEnergy var_H(0., SA.get_mu(), SA.get_sigma(), 2.5*SA.get_sigma(), &rnd);

    int N = 100, L = 5000;
    vector<double> sum_H(N), sum2_H(N), error_H(N);
    // Psi histogram variables
    int nbins = 100;
    double bin_start = - SA.get_mu() - 4*SA.get_sigma();
    double bin_size = -2. * bin_start / static_cast<double>(nbins);
    vector<double> psi_averages(nbins, 0.), psi_averages2(nbins, 0.), psi_errors(nbins);

    for (int i = 0; i < N; i++) {
        double sum = 0.;
        vector<int> psi_block(nbins, 0);
        for (int j = 0; j < L; j++) {
            var_H.metro();
            sum += var_H.H_psi_T();

            // If x is in the bounds of the histogram add 1 to the corresponding bin
            if (var_H.get_x() >= bin_start && var_H.get_x() < -bin_start) {
                int histo_index = static_cast<int>((var_H.get_x() - bin_start) / bin_size);
                psi_block[histo_index]++;
            }

        }

        sum_H[i] = sum / static_cast<double>(L);
        sum2_H[i] = sum_H[i]*sum_H[i];

        // The cumulative averages are not saved since only the final error is needed
        for (int j = 0; j < nbins; j++) {
            psi_averages[j] += psi_block[j] / static_cast<double>(L);
            psi_averages2[j] += pow(psi_block[j] / static_cast<double>(L), 2);
        }
    }

    // Evaluates averages and uncertainties
    averages_from_blocks(sum_H, sum2_H, error_H);
    write_data(sum_H, error_H, "OUTPUT/optimal_energy.out");
    for (int i = 0; i < nbins; i++) {
        psi_averages[i] /= static_cast<double>(N);
        psi_averages2[i] /= static_cast<double>(N);
        psi_errors[i] = sqrt((psi_averages2[i] - psi_averages[i]*psi_averages[i]) / static_cast<double>(N-1));
    }

    // Write the histogram data
    ofstream output("OUTPUT/psi_distribution.out");
    if (output.is_open()) {
        output << "#      BINS:     COUNTS:     ERRORS:\n";
        for (int i = 0; i < nbins; i++) {
            output << setw(12) << i*bin_size + bin_start << setw(12) << psi_averages[i] / bin_size 
                   << setw(12) << psi_errors[i] / bin_size << endl;
        }
    } else cerr << "ERROR: Cannot open OUTPUT/psi_distribution.out\n";

    cout << "Final values: Mu=" << SA.get_mu() << "\t Sigma=" << SA.get_sigma()
         << "\t Energy=" << sum_H[N-1] << "+-" << error_H[N-1] << endl;

    return 0;
}