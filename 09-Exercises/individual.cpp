#include "individual.h"

Individual :: Individual(vector<int> chromosome) {
    _chromosome = chromosome;
    _n_genes = chromosome.size();
}

void Individual :: compute_fitness(const vector<vector<double>>& city_coordinates) {
    double fitness = 0.;
    int n_dim = city_coordinates[0].size();
    double distance;
    int index1, index2;

    for (int i = 0; i < _n_genes-1; i++) {
        distance = 0.;
        index1 = _chromosome[i] - 1;
        index2 = _chromosome[i+1] - 1;
        for (int j = 0; j < n_dim; j++) {
            distance += pow(city_coordinates[index1][j] - city_coordinates[index2][j], 2);
        }
        fitness += sqrt(distance);
    }
    _fitness = fitness;
}

void Individual :: pair_permutation(int a, int b) {
    if (a == b) return;
    _chromosome[a] ^= _chromosome[b];
    _chromosome[b] ^= _chromosome[a];
    _chromosome[a] ^= _chromosome[b];
}

bool Individual :: check_constraints() {
    if (_chromosome[0] != 1) return false;
    int sum = 1;
    for (int i = 1; i <_n_genes; i++) sum += _chromosome[i];
    return sum == (_n_genes+1)*_n_genes/2;
}