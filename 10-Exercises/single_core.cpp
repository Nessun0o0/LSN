#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes", 0);
    
    int n_cities, n_individuals, n_generations, n_data;
    
    // Input the parameters from file
    ifstream input("input_single.dat");
    if (input.is_open()) {
        char tmp[512];
        for (int i = 0; i < 5; i++) input >> tmp;
        input >> n_cities >> n_individuals >> n_generations >> n_data;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

    // Reads the city coordinates from file
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

        vector<double> lengths(n_data+1);
        int n_save = n_generations / n_data;

        for (int i = 0; i < n_generations; i++) {
           pop.evolve_population();

           if (i%n_save == 0) lengths[i/n_save] = pop.get_elite().get_fitness(); 
        }

        Individual elite = pop.get_elite();
        lengths[n_data] = elite.get_fitness();

        ofstream output("OUTPUT/best_single.out");
        if (output.is_open()) {
            output << "#   CITIES\n";
            for (int j = 0; j < n_cities; j++) {
                output << setw(10) << elite.get_chromosome()[j] << "\n";      
            }
        } else cerr << "ERROR: Cannot open OUTPUT/best_single.out\n";
        output.close();

        output.open("OUTPUT/history_single.out");
        if (output.is_open()) {
            output << "#     STEP   BEST_LENGTH\n";
            for (int j = 0; j <= n_data; j++) {
                output << setw(10) << j * n_save + 1 << setw(14) << lengths[j] << "\n";      
            }
        } else cerr << "ERROR: Cannot open OUTPUT/history_single.out\n";
        output.close();
        
        
        cout << "Optimal solution length: " << elite.get_fitness() << endl;
        

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    return 0;
}