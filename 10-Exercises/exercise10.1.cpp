#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "mpi.h"

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    vector<MPI_Status> status(size-1);
    vector<MPI_Request> requests(size-1);

    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes", rank);
    
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

        Population pop(n_individuals, n_cities, city_coordinates, rank+0.1, &rnd);

        for (int i = 0; i < n_generations; i++) {
            pop.metropolis_move();
            bool exchange = false;
            vector<int> next_chromosome(n_cities);
            if (rank != 0) MPI_Send(&pop.get_population()[0].get_chromosome()[0], n_cities, MPI_INTEGER, rank-1, rank-1, MPI_COMM_WORLD, &requests[rank-1]);
            if (rank != size-1) {
                MPI_Recv(&next_chromosome[0], n_cities, MPI_INTEGER, rank+1, rank, MPI_COMM_WORLD, &status[rank]);
                Individual next_individual(next_chromosome);
                next_individual.compute_fitness(city_coordinates);
                double next_beta = 5.; // TEMPORANEO
                if (rnd.Rannyu() < exp(-(pop.get_beta() - next_beta)*(next_individual.get_fitness() - pop.get_population()[0].get_fitness()))) {
                    exchange = true;
                }
                MPI_Send(&exchange, 1, MPI_LOGICAL, rank+1, rank+1, MPI_COMM_WORLD, &requests[rank-1]);
                if (exchange) {
                    MPI_Send(&pop.get_population()[0].get_chromosome()[0], n_cities, MPI_INTEGER, rank+1, rank+1, MPI_COMM_WORLD, &requests[rank-1]);
                    pop.get_population()[0].set_chromosome(next_chromosome);
                }
            }    

        }

        Individual elite = pop.get_elite();

        for (int j = 0; j < n_cities; j++) {

            cout << elite.get_chromosome()[j] << " ";      
        }
        cout << endl << elite.get_fitness() << endl;

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    MPI_Finalize();
    return 0;
}