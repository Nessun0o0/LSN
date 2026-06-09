#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>
#include "mpi.h"

#include "random.h"
#include "population.h"


using namespace std;

int main(int argc, char *argv[]) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Each rank initializes the RNG with a different seed
    Random rnd;
    rnd.SetSeedFromFile("seed.in", "Primes", rank);
    
    int n_cities, n_individuals, n_generations, n_data;
    double t_max, t_min;
    
    ifstream input("input.dat");
    if (input.is_open()) {
        char tmp[512];
        for (int i = 0; i < 7; i++) input >> tmp;
        input >> n_cities >> n_individuals >> n_generations >> t_max >> t_min >> n_data;
    } else {
        cerr << "ERROR: can't open input.dat" << endl;
        exit(-1);
    }
    input.close();

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

        int n_swap = 10; // Number of steps between each swap move
        double r = pow(t_max / t_min, 1/static_cast<double>(size-1));
        Population pop(n_individuals, n_cities, city_coordinates, t_min*pow(r, rank), &rnd);

        int n_acc = 0; // To compute the acceptance rate of the swap
        vector<double> lengths;
        int n_save = n_generations/n_data;
        if (rank == 0) lengths.resize(n_data);

        vector<MPI_Status> status(size-1);

        for (int i = 0; i < n_generations; i++) {
            pop.metropolis_move();
            if (rank == 0 && i%n_save == 0) lengths[i/n_save] = pop.get_elite().get_fitness();
            if (i%n_swap != 0 || i == 0) continue;

            // Chromosome exchange move
            MPI_Request reqs[3];
            bool exchange = false;
            vector<int> next_chromosome(n_cities);
            vector<int> current_chromosome = pop.get_population()[0].get_chromosome();
            double current_fitness = pop.get_population()[0].get_fitness();

            // Send the current chromosome to the next cooler temperature 
            if (rank != 0) MPI_Isend(&current_chromosome[0], n_cities, MPI_INT, rank-1, rank-1, MPI_COMM_WORLD, &reqs[0]);
            
            // Receive the chromosome from the next hotter temperature and try an exchange move
            if (rank != size-1) {
                MPI_Recv(&next_chromosome[0], n_cities, MPI_INT, rank+1, rank, MPI_COMM_WORLD, &status[rank]);
                Individual next_individual(next_chromosome);
                next_individual.compute_fitness(city_coordinates);

                double next_beta = 1./(t_min*pow(r, rank+1));
                double acceptance = exp(-(pop.get_beta() - next_beta)*(next_individual.get_fitness() - current_fitness));

                if (rnd.Rannyu() < acceptance) exchange = true;
                // Send the signal to the hotter temperature that the exchange was accepted
                MPI_Isend(&exchange, 1, MPI_CXX_BOOL, rank+1, rank+1, MPI_COMM_WORLD, &reqs[1]);
                if (exchange) {
                    n_acc++;
                    // Send the current chromosome to the hotter temperature
                    MPI_Isend(&current_chromosome[0], n_cities, MPI_INT, rank+1, rank+1, MPI_COMM_WORLD, &reqs[2]);
                    pop.get_population()[0].set_chromosome(next_chromosome);
                }
            }
            if (rank != 0) {
                
                // Receive the exchange signal from the cooler temperature
                bool exchange_signal;
                MPI_Recv(&exchange_signal, 1, MPI_CXX_BOOL, rank-1, rank, MPI_COMM_WORLD, &status[rank-1]);
                
                if (exchange_signal) {
                    // Swap the current chromosome
                    vector<int> exchange_chromosome(n_cities);
                    MPI_Recv(&exchange_chromosome[0], n_cities, MPI_INT, rank-1, rank, MPI_COMM_WORLD, &status[rank-1]);
                    pop.get_population()[0].set_chromosome(exchange_chromosome);
                }
                
                MPI_Wait(&reqs[0], &status[rank-1]);
            }

            if (rank != size-1) {
                MPI_Wait(&reqs[1], &status[rank]);
                if (exchange) MPI_Wait(&reqs[2], &status[rank]);
            }
            
        }

        // Calculate acceptance rate
        double acc = (static_cast<double>(n_acc) * n_swap) / n_generations;
        double* acceptances = new double[size];
        MPI_Gather(&acc, 1, MPI_DOUBLE, acceptances, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Output optimal solution
        if (rank == 0) {
            Individual elite = pop.get_elite();
            lengths.push_back(elite.get_fitness());

            ofstream output("OUTPUT/best_PT.out");
            if (output.is_open()) {
                output << "#   CITIES\n";
                for (int j = 0; j < n_cities; j++) {
                    output << setw(10) << elite.get_chromosome()[j] << "\n";      
                }
            } else cerr << "ERROR: Cannot open OUTPUT/best_PT.out\n";
            output.close();

            output.open("OUTPUT/history_PT.out");
            if (output.is_open()) {
                output << "#     STEP   BEST_LENGTH\n";
                for (int j = 0; j <= n_data; j++) {
                    output << setw(10) << j * n_save + 1 << setw(14) << lengths[j] << "\n";      
                }
            } else cerr << "ERROR: Cannot open OUTPUT/history_PT.out\n";
            output.close();
            
            
            cout << "Optimal solution length: " << elite.get_fitness() << endl;

        }
        delete[] acceptances;

    } catch (const exception& err) {
        cerr << err.what() << endl;
        exit(-1);
    }

    MPI_Finalize();
    return 0;
}