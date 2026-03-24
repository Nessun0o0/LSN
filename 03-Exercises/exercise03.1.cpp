#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "random.h"

// Computes the averages and their errors from the data blocks and saves them to the vectors passed as argument
void averages_from_blocks(const std::vector<double>& data_blocks, const std::vector<double>& data_blocks_squared, std::vector<double>& averages, std::vector<double>& errors) {
    int N = data_blocks.size();
    std::vector<double> cumsum(N), cumsum_squared(N);

    cumsum[0] = data_blocks[0];
    cumsum_squared[0] = data_blocks_squared[0];
    averages[0] = data_blocks[0];
    errors[0] = 0.;
    for (int i = 1; i < N; i++) {
        cumsum[i] = cumsum[i-1] + data_blocks[i];
        cumsum_squared[i] = cumsum_squared[i-1] + data_blocks_squared[i];

        averages[i] = cumsum[i] / static_cast<double>(i+1);
        errors[i] = std::sqrt((cumsum_squared[i]/(i+1) - std::pow(averages[i],2))/i);
    }
}

int write_data(const std::vector<double>& averages, const std::vector<double>& errors, std::string filename){

    std::ofstream outfile;
    outfile.open(filename);
    if (outfile.is_open()) {
        for (int i = 0; i < averages.size(); i++) {
            outfile << i+1 << " " << averages[i] << " " << errors[i] << std::endl;
        }
    } else {
        std::cerr << "Error: unable to write " + filename << std::endl;
        return -1;
    }
    return 0;
}

int main (int argc, char *argv[]){

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");

    const double S_0 = 100.;
    const double T = 1.;
    const double K = 100;
    const double r = 0.1;
    const double sigma = 0.25;

    const int M = 10000;
    const int N = 100;
    const int L = M/N;
    const int time_steps = 100;
    const double deltaT = T / static_cast<double>(time_steps);

    //std::vector<double> call_prices(N), call_prices2(N);
    //std::vector<double> put_prices(N), put_prices2(N);
    
    std::vector<double> call_prices_direct(N), call_prices2_direct(N);
    std::vector<double> put_prices_direct(N), put_prices2_direct(N);
    std::vector<double> call_prices_discrete(N), call_prices2_discrete(N);
    std::vector<double> put_prices_discrete(N), put_prices2_discrete(N);

    for (int i = 0; i < N; i++) {
        double sum_C = 0.;
        double sum_P = 0.;

        for (int j = 0; j < L; j++) {
            double W = rnd.Gauss(0., std::sqrt(T));
            double S = S_0*std::exp((r-std::pow(sigma,2)/2.)*T + sigma*W);
            double C = std::exp(-r*T)*std::max(0., S-K);
            double P = std::exp(-r*T)*std::max(0., K-S);
            sum_C += C;
            sum_P += P;
        }

        call_prices_direct[i] = sum_C / static_cast<double>(L);
        call_prices2_direct[i] = std::pow(call_prices_direct[i], 2);
        put_prices_direct[i] = sum_P / static_cast<double>(L);
        put_prices2_direct[i] = std::pow(put_prices_direct[i], 2);

        // Discrete call prices
        double sum_C_discrete = 0.;
        double sum_P_discrete = 0.;
    
        for (int j = 0; j < L; j++) {
            double S = S_0;
            for (int k = 0; k < time_steps; k++) {
                double Z = rnd.Gauss(0., 1.);
                S = S*std::exp((r-std::pow(sigma,2)/2.)*deltaT + sigma*Z*std::sqrt(deltaT));
            }
            sum_C_discrete += std::exp(-r*T)*std::max(0., S-K);
            sum_P_discrete += std::exp(-r*T)*std::max(0., K-S);
        }

        call_prices_discrete[i] = sum_C_discrete / static_cast<double>(L);
        call_prices2_discrete[i] = std::pow(call_prices_discrete[i], 2);
        put_prices_discrete[i] = sum_P_discrete / static_cast<double>(L);
        put_prices2_discrete[i] = std::pow(put_prices_discrete[i], 2);
    }

    std::vector<double> call_averages_direct(N), call_errors_direct(N);
    std::vector<double> put_averages_direct(N), put_errors_direct(N);
    std::vector<double> call_averages_discrete(N), call_errors_discrete(N);
    std::vector<double> put_averages_discrete(N), put_errors_discrete(N);

    averages_from_blocks(call_prices_direct, call_prices2_direct, call_averages_direct, call_errors_direct);
    averages_from_blocks(put_prices_direct, put_prices2_direct, put_averages_direct, put_errors_direct);
    averages_from_blocks(call_prices_discrete, call_prices2_discrete, call_averages_discrete, call_errors_discrete);
    averages_from_blocks(put_prices_discrete, put_prices2_discrete, put_averages_discrete, put_errors_discrete);

    write_data(call_averages_direct, call_errors_direct, "call_direct.out");
    write_data(put_averages_direct, put_errors_direct, "put_direct.out");
    write_data(call_averages_discrete, call_errors_discrete, "call_discrete.out");
    write_data(put_averages_discrete, put_errors_discrete, "put_discrete.out");

    /*std::ofstream outfile;

    outfile.open("importance.out");
    if (outfile.is_open()) {
        for (int i = 0; i < N; i++) {
            outfile << N*i << " " << cumsum_importance[i] << " " << errors_importance[i] << std::endl;
        }
    } else std::cerr << "Error: unable to write importance.out" << std::endl;
    outfile.close();*/

   return 0;
}