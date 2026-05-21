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

    
    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes", rank);
    
    int n_cities, n_individuals, n_generations;
    double d_temp;
    
    ifstream input("input.dat");
    if (input.is_open()) {
        input >> n_cities >> n_individuals >> n_generations >> d_temp;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

    vector<vector<double>> city_coordinates(n_cities, vector<double>(2, 0.));

    if (rank == 0) {
        input.open("cap_prov_ita.dat");
        if (input.is_open()) {
            for (int i = 0; i < n_cities; i++) {
                //input >> x >> y;
                input >> city_coordinates[i][0] >> city_coordinates[i][1];
                //city_coordinates[i] = vector<double>{x,y};
            }
        } else {
            cerr << "ERROR: can't open cap_prov_ita.dat" << endl;
            exit(-1);
        }
    }
    for (int i = 0; i < n_cities; i++) MPI_Bcast(&city_coordinates[i][0], 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    try {

        Population pop(n_individuals, n_cities, city_coordinates, rank*d_temp+0.1, &rnd);

        for (int i = 0; i < n_generations; i++) {
            pop.metropolis_move();
            MPI_Status status;
            bool exchange = false;
            vector<int> next_chromosome(n_cities);
            if (rank != 0) MPI_Send(&pop.get_population()[0].get_chromosome()[0], n_cities, MPI_INT, rank-1, rank-1, MPI_COMM_WORLD);
            if (rank != size-1) {
                MPI_Recv(&next_chromosome[0], n_cities, MPI_INT, rank+1, rank, MPI_COMM_WORLD, &status);
                Individual next_individual(next_chromosome);
                next_individual.compute_fitness(city_coordinates);
                double next_beta = 1./(0.1+d_temp*(rank+1)); // TEMPORANEO
                double acceptance = exp(-(pop.get_beta() - next_beta)*(next_individual.get_fitness() - pop.get_population()[0].get_fitness()));
                if (rnd.Rannyu() < acceptance) exchange = true;
                MPI_Send(&exchange, 1, MPI_CXX_BOOL, rank+1, rank+1, MPI_COMM_WORLD);
                if (exchange) {
                    MPI_Send(&pop.get_population()[0].get_chromosome()[0], n_cities, MPI_INT, rank+1, rank+1, MPI_COMM_WORLD);
                    //cout << "Exchange between " << rank << " and " << rank+1 << " with acceptance " << acceptance << endl;
                    pop.get_population()[0].set_chromosome(next_chromosome);
                }
            }
            if (rank != 0) {
                bool exchange_signal;
                MPI_Recv(&exchange_signal, 1, MPI_CXX_BOOL, rank-1, rank, MPI_COMM_WORLD, &status);
                if (exchange_signal) {
                    vector<int> exchange_chromosome(n_cities);
                    MPI_Recv(&exchange_chromosome[0], n_cities, MPI_INT, rank-1, rank, MPI_COMM_WORLD, &status);
                    pop.get_population()[0].set_chromosome(exchange_chromosome);
                }
            }

        }

        if (rank == 0) {
            Individual elite = pop.get_elite();

            ofstream output("output.out");
            if (output.is_open()) {
                for (int j = 0; j < n_cities; j++) {

                    output << elite.get_chromosome()[j] << "\n";      
                }
            }
            cout << elite.get_fitness() << endl;
        }

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    MPI_Finalize();
    return 0;
}