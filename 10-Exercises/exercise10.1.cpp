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
    double t_max, t_min;
    
    ifstream input("input.dat");
    if (input.is_open()) {
        input >> n_cities >> n_individuals >> n_generations >> t_max >> t_min;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();
    //double d_beta = (1./t_min - 1./t_max) / (size-1);

    vector<vector<double>> city_coordinates(n_cities, vector<double>(2, 0.));

    if (rank == 0) {
        // Loads all the city coordinates into an array, then sends it to all the other ranks
        input.open("cap_prov_ita.dat");
        if (input.is_open()) {
            for (int i = 0; i < n_cities; i++) {
                input >> city_coordinates[i][0] >> city_coordinates[i][1];
            }
        } else {
            cerr << "ERROR: can't open cap_prov_ita.dat" << endl;
            exit(-1);
        }
    }
    for (int i = 0; i < n_cities; i++) MPI_Bcast(&city_coordinates[i][0], 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    try {

        double r = pow(t_max / t_min, 1/static_cast<double>(size-1));
        Population pop(n_individuals, n_cities, city_coordinates, t_min*pow(r, rank), &rnd);
        //Population pop(n_individuals, n_cities, city_coordinates, 1.0 / (1./t_min - rank * d_beta), &rnd);
        //Population pop(n_individuals, n_cities, city_coordinates, 0.1+rank*d_temp, &rnd);
        int n_acc = 0;

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
                //double next_beta = 1./(0.1+d_temp*(rank+1)); // TEMPORANEO
                double next_beta = 1./(t_min*pow(r, rank+1));
                //double next_beta = pop.get_beta() - d_beta;
                double acceptance = exp(-(pop.get_beta() - next_beta)*(next_individual.get_fitness() - pop.get_population()[0].get_fitness()));

                /* double denominator = 1e-3 * pow(r, rank+1);
    double next_beta_calc = 1. / denominator;
    cout << "rank " << rank << " pow(r,rank+1)=" << pow(r, rank+1) 
         << " denominator=" << denominator 
         << " next_beta=" << next_beta_calc << endl; */


                if (rnd.Rannyu() < acceptance) exchange = true;
                MPI_Send(&exchange, 1, MPI_CXX_BOOL, rank+1, rank+1, MPI_COMM_WORLD);
                if (exchange) {
                    n_acc++;
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
        double acc = static_cast<double>(n_acc) / n_generations;
        double* acceptations = new double[size];
        MPI_Gather(&acc, 1, MPI_DOUBLE, acceptations, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            Individual elite = pop.get_elite();

            ofstream output("output.out");
            if (output.is_open()) {
                for (int j = 0; j < n_cities; j++) {

                    output << elite.get_chromosome()[j] << "\n";      
                }
            }
            cout << elite.get_fitness() << endl;

            for (int i = 0; i < size; i++) cout << acceptations[i] << " ";
            cout << endl;

        }
        delete[] acceptations;

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    MPI_Finalize();
    return 0;
}