#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");
    
    double n_cities, n_individuals, n_generations;
    ifstream input("input.dat");
    if (input.is_open()) {
        input >> n_cities >> n_individuals >> n_generations;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

    vector<vector<double>> city_coordinates(n_cities);

    // Place cities on a circumference
    double dtheta = 2*M_PI / static_cast<double>(n_cities);
    for (int i = 0; i < n_cities; i++) {
        vector<double> coordinate(2);
        coordinate[0] = cos(dtheta*i);
        coordinate[1] = sin(dtheta*i);
        city_coordinates[i] = coordinate;
    }
    try {
        Population pop(n_individuals, n_cities, city_coordinates, &rnd);
        
        /* vector<Individual> starting_pop = pop.get_population();
        for (Individual individual : starting_pop) {
            for (int city : individual.get_chromosome()) cout << city << " ";
            cout << endl;
        }
        cout << endl; */

        for (int i = 0; i < n_generations; i++) {
            pop.evolve_population();
        }

        /* for (Individual individual : starting_pop) {
            for (int city : individual.get_chromosome()) cout << city << " ";
            cout << endl;
        }
        cout << endl; */

        Individual elite = pop.get_elite();

        for (int j = 0; j < n_cities; j++) {

            cout << elite.get_chromosome()[j] << " ";      
        }
        cout << endl << elite.get_fitness() << endl;

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    for (int i = 0; i < n_cities; i++) {
        city_coordinates[i] = {rnd.Rannyu(-1.,1.), rnd.Rannyu(-1.,1.)};
    }
    try {
        Population pop(n_individuals, n_cities, city_coordinates, &rnd);

        for (int i = 0; i < n_generations; i++) {
            pop.evolve_population();
        }

        /* for (Individual individual : starting_pop) {
            for (int city : individual.get_chromosome()) cout << city << " ";
            cout << endl;
        }
        cout << endl; */

        Individual elite = pop.get_elite();
        ofstream output("square.out");

        for (int j = 0; j < n_cities; j++) {

            output << elite.get_chromosome()[j] << " ";
            cout << elite.get_chromosome()[j] << " ";      
        }
        output << endl;
        for (int i = 0; i < n_cities; i++) output << city_coordinates[i][0] << " " << city_coordinates[i][1] << endl;
        cout << endl << elite.get_fitness() << endl;

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    return 0;
}