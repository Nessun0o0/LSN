#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <fstream>

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes");
    
    int n_cities, n_individuals, n_generations;
    bool circle_simulation;
    // Input the parameters from file
    ifstream input("input.dat");
    if (input.is_open()) {
        char tmp[512];
        for (int i = 0; i < 5; i++) input >> tmp;
        input >> n_cities >> n_individuals >> n_generations >> circle_simulation;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

    vector<vector<double>> city_coordinates(n_cities);
    ofstream output;

    // Generate the city configuration
    if (circle_simulation) {
        // Place the cities at random on a circumference
        for (int i = 0; i < n_cities; i++) {
            double theta = rnd.Rannyu()*2.*M_PI;
            vector<double> coordinate(2);
            coordinate[0] = cos(theta);
            coordinate[1] = sin(theta);
            city_coordinates[i] = coordinate;
        }
    } else {
        // Place the cities at random inside a square
        for (int i = 0; i < n_cities; i++) {
            city_coordinates[i] = {rnd.Rannyu(-1.,1.), rnd.Rannyu(-1.,1.)};
        }
    }

    try {
        Population pop(n_individuals, n_cities, city_coordinates, &rnd);

        vector<double> path_averages(n_generations, 0.), best_paths(n_generations), path_errors(n_generations);

        // Evolve the population
        for (int i = 0; i < n_generations; i++) {
            pop.evolve_population();
            best_paths[i] = pop.get_elite().get_fitness(); // Saves the length of the best path of each generation

            // Compute average and standard deviation of the best half of the population
            double sum2 = 0.;
            int half = n_individuals/2;
            for (int j = 0; j < half; j++) {
                path_averages[i] += pop.get_population()[j].get_fitness();
                sum2 += pop.get_population()[j].get_fitness() * pop.get_population()[j].get_fitness();
            }
            path_averages[i] /= static_cast<double>(half);
            path_errors[i] = sqrt((sum2 - static_cast<double>(half)*path_averages[i]*path_averages[i])/static_cast<double>(half-1)); // Standard deviation
        }

        // Output the individual containing the best solution
        Individual elite = pop.get_elite();

        // Write the order of the best path and its length
        if (circle_simulation) output.open("OUTPUT/circle_path.out");
        else output.open("OUTPUT/square_path.out");

        if (output.is_open()) {
            output << "# CITIES_ORDER   PATH_LENGTH\n"
                   << setw(14) << elite.get_chromosome()[0] << setw(14) << elite.get_fitness() << "\n";
            for (int j = 0; j < n_cities; j++) {
                output << setw(14) << elite.get_chromosome()[j] << "\n";      
            }
        } else cerr << "ERROR: Cannot open output file";
        output.close();

        // Write the evolution of the best path in different generations
        if (circle_simulation) output.open("OUTPUT/circle_evolution.out");
        else output.open("OUTPUT/square_evolution.out");

        if (output.is_open()) {
            output << "#   GENERATION   BEST_LENGTH     BEST_HALF       STD_DEV\n";
            for (int i = 0; i < n_generations; i++) {
                output << setw(14) << i+1 << setw(14) << best_paths[i] << setw(14) << path_averages[i]
                       << setw(14) << path_errors[i] << "\n";      
            }
        } else cerr << "ERROR: Cannot open output file";
        output.close();

        // Write the coordinates of the cities
        if (circle_simulation) output.open("OUTPUT/circle_coordinates.out");
        else output.open("OUTPUT/square_coordinates.out");

        if (output.is_open()) {
            output << "#         CITY       COORD_X       COORD_Y\n";
            for (int i = 0; i < n_cities; i++) {
                output << setw(14) << i+1 << setw(14) << city_coordinates[i][0] << setw(14) << city_coordinates[i][1] << "\n";
            }
        } else cerr << "ERROR: Cannot open output file";
        output.close();



    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    return 0;
}