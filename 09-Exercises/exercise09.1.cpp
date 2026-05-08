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
    
    double n_cities, n_individuals;
    ifstream input("input.dat");
    if (input.is_open()) {
        input >> n_cities >> n_individuals;
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
        vector<Individual> ciao = pop.get_population();
        //cout << pop.get_population() << endl;
        for (int i = 0; i < n_individuals; i++) {
            cout << ciao[i].check_constraints() << " ";
            for (int j = 0; j < n_cities; j++) {
    
                cout << ciao[i].get_chromosome()[j] << " ";      
            }
            cout << endl;
        }
    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }


    return 0;
}