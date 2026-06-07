#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes", 0);
    
    int n_cities, n_individuals, n_generations;
    
    ifstream input("input_single.dat");
    if (input.is_open()) {
        input >> n_cities >> n_individuals >> n_generations;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

    vector<vector<double>> city_coordinates(n_cities, vector<double>(2, 0.));

    input.open("cap_prov_ita.dat");
    if (input.is_open()) {
        for (int i = 0; i < n_cities; i++) {
            input >> city_coordinates[i][0] >> city_coordinates[i][1];
        }
    } else {
        cerr << "ERROR: can't open cap_prov_ita.dat" << endl;
        exit(-1);
    }

    try {

        Population pop(n_individuals, n_cities, city_coordinates, 1., &rnd);

        for (int i = 0; i < n_generations; i++) {
           pop.evolve_population();
        }

        Individual elite = pop.get_elite();

        ofstream output("OUTPUT/single_core.out");
        if (output.is_open()) {
            for (int j = 0; j < n_cities; j++) {

                output << elite.get_chromosome()[j] << "\n";      
            }
        }
        cout << elite.get_fitness() << endl;
        

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    return 0;
}