#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "variationalMC.h"


using namespace std;

// Computes cumulative averages and errors with the data blocking technique
// The cumulative averages are inserted in the vector data_blocks, the errors in the vector errors
void averages_from_blocks(vector<double>& data_blocks, const vector<double>& data_blocks_squared, vector<double>& errors) {
    int N = data_blocks.size();
    vector<double> cumsum(N), cumsum_squared(N);

    cumsum[0] = data_blocks[0];
    cumsum_squared[0] = data_blocks_squared[0];
    errors[0] = 0.;
    for (int i = 1; i < N; i++) {
        cumsum[i] = cumsum[i-1] + data_blocks[i];
        cumsum_squared[i] = cumsum_squared[i-1] + data_blocks_squared[i];

        data_blocks[i] = cumsum[i] / static_cast<double>(i+1);
        errors[i] = sqrt((cumsum_squared[i]/(i+1) - pow(data_blocks[i],2))/i);
    }
}

int write_data(const vector<double>& averages, const vector<double>& errors, string filename){

    ofstream outfile;
    outfile.open(filename);
    if (outfile.is_open()) {
        for (int i = 0; i < int(averages.size()); i++) {
            outfile << i+1 << " " << averages[i] << " " << errors[i] << endl;
        }
    } else {
        cerr << "Error: unable to write " + filename << endl;
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");
    
    double mu, sigma, delta_SA, delta_H, T, cooling_rate;
    int n_sim;
    ifstream input("input_2.dat");
    if (input.is_open()) {
        input >> mu >> sigma >> delta_SA >> delta_H >> T >> cooling_rate >> n_sim;
    } else {
        cerr << "ERROR: can't open input_1.dat" << endl;
        exit(-1);
    }
    input.close();

    /* T = 5;
    mu = 0.783381;
    sigma = 0.6346; */
    SimulatedAnnealing SA(mu, sigma, T, delta_SA, &rnd);
    SA.set_delta_H(delta_H);
    while (T > 1e-3) {
        SA.reset_acceptance();
        for (int i = 0; i < n_sim; i++) {
            SA.metro();
        }
        cout << SA.get_acceptance() << "\t" << SA.get_H_acceptance() << "\t" << SA.get_mu() << "\t" << SA.get_sigma() << endl;
        if (SA.get_acceptance() > 0.55) SA.set_delta_SA(SA.get_delta()*1.2);
        else if (SA.get_acceptance() < 0.45) SA.set_delta_SA(SA.get_delta()/1.2);

        T *= cooling_rate;
        SA.set_compute_H_steps(10000 + 100/T);
        //T -= dT;
        SA.set_temp(T);
    }

    ExpectationEnergy var_H(0., SA.get_mu(), SA.get_sigma(), 2.5*SA.get_sigma(), &rnd);
    int N = 100, L = 1000;
    vector<double> sum_H(N), sum2_H(N), error_H(N);

    for (int i = 0; i < N; i++) {
        double sum = 0.;
        for (int j = 0; j < L; j++) {
            var_H.metro();
            sum += var_H.H_psi_T();
        }
        sum_H[i] = sum / static_cast<double>(L);
        sum2_H[i] = sum_H[i]*sum_H[i];
    }

    averages_from_blocks(sum_H, sum2_H, error_H);
    write_data(sum_H, error_H, "OUTPUT/output_2.out");

    cout << SA.get_mu() << " " << SA.get_sigma() << " " << sum_H[N-1] << " +- " << error_H[N-1] << endl;

    return 0;
}