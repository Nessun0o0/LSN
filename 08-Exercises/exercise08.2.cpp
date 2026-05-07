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
    
    double mu, sigma, delta_SA, delta_H, T, dT;
    int n_sim;
    ifstream input("input_2.dat");
    if (input.is_open()) {
        input >> mu >> sigma >> delta_SA >> delta_H >> T >> dT >> n_sim;
    } else {
        cerr << "ERROR: can't open input_1.dat" << endl;
        exit(-1);
    }
    input.close();

    T = 5;
    mu = 0.783381;
    sigma = 0.6346;
    SimulatedAnnealing SA(mu, sigma, T, delta_SA, &rnd);
    SA.set_delta_H(delta_H);
    while (T > 0) {
        SA.reset_acceptance();
        for (int i = 0; i < n_sim; i++) {
            SA.metro();
        }
        cout << SA.get_acceptance() << "\t" << SA.get_tries() << "\t" << SA.get_mu() << "\t" << SA.get_sigma() << endl;
        T -= dT;
        SA.set_temp(T);
    }
    cout << SA.get_mu() << " " << SA.get_sigma() << " " << SA.get_H() << endl;

    return 0;
}