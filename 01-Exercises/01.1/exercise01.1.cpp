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

   // Part 1

   double M = 10000;
   double N = 100;
   double L = M/N;
   std::vector<double> averages(N), averages2(N);
   std::vector<double> cumsum_averages(N), cumsum_averages2(N), errors_averages(N);

   for (int i = 0; i < N; i++) {
    double sum = 0.;
    for (int j = 0; j < L; j++) {
        sum += rnd.Rannyu();
    }

    averages[i] = sum / static_cast<double>(L);
    averages2[i] = averages[i] * averages[i];
   }

   cumsum_averages = Cumsum(averages);
   cumsum_averages2 = Cumsum(averages2);

   for (int i = 0; i < N; i++) {
    cumsum_averages[i] /= static_cast<double>(i + 1);
    cumsum_averages2[i] /= static_cast<double>(i + 1);
    if (i == 0) {
        errors_averages[i] = 0.;
        continue;
    }
    errors_averages[i] = std::sqrt((cumsum_averages2[i] - std::pow(cumsum_averages[i], 2)) / static_cast<double>(i));
   }

   // Write results to file
   std::ofstream out;
   out.open("averages.out");
   if (out.is_open()) {
    for (int i = 0; i < N; i++) {
        out << cumsum_averages[i] << " " << errors_averages[i] << std::endl;
    }
   } else std::cerr << "Error: unable to write averages.out" << std::endl;
   out.close();

   // Part 2

   std::vector<double> variances(N), variances2(N);
   std::vector<double> cumsum_variances(N), cumsum_variances2(N), errors_variances(N);

   for (int i = 0; i < N; i++) {
    double sum = 0.;
    for (int j = 0; j < L; j++) {
        sum += std::pow(rnd.Rannyu() - 0.5, 2);
    }

    variances[i] = sum / static_cast<double>(L);
    variances2[i] = variances[i] * variances[i];
   }

   cumsum_variances = Cumsum(variances);
   cumsum_variances2 = Cumsum(variances2);

   for (int i = 0; i < N; i++) {
    cumsum_variances[i] /= static_cast<double>(i + 1);
    cumsum_variances2[i] /= static_cast<double>(i + 1);
    if (i == 0) {
        errors_variances[i] = 0.;
        continue;
    }
    errors_variances[i] = std::sqrt((cumsum_variances2[i] - std::pow(cumsum_variances[i], 2)) / static_cast<double>(i));
   }

   // Write results to file
   out.open("variances.out");
   if (out.is_open()) {
    for (int i = 0; i < N; i++) {
        out << cumsum_variances[i] << " " << errors_variances[i] << std::endl;
    }
   } else std::cerr << "Error: unable to write variances.out" << std::endl;
   out.close();

   // Part 3
   M = 100;
   N = 10000;
   int n = 10000;
   double expected = static_cast<double>(n) / static_cast<double>(M);
   std::vector<double> chi2(N);

   for (int i = 0; i < N; i++) {
    std::vector<int> counts(M, 0);
    for (int j = 0; j < n; j++) counts[static_cast<int>(rnd.Rannyu(0., M))]++;

    double sum = 0.;
    for (int count : counts) {
        sum += std::pow(count - expected, 2);
    }

    chi2[i] = sum / expected;
   }

   // Write results to file
   out.open("chi2.out");
   if (out.is_open()) {
    for (int i = 0; i < N; i++) {
        out << chi2[i] << std::endl;
    }
   } else std::cerr << "Error: unable to write chi2.out" << std::endl;
   out.close();

   return 0;
}