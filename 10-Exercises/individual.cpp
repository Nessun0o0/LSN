#include "individual.h"

Individual :: Individual(vector<int> chromosome) {
    _chromosome = chromosome;
    _n_genes = chromosome.size();
}

void Individual :: compute_fitness(const vector<vector<double>>& city_coordinates) {
    double fitness = 0.;
    int n_dim = city_coordinates[0].size();
    double distance = 0.;
    int index1, index2;

    for (int i = 0; i < _n_genes-1; i++) {
        index1 = _chromosome[i] - 1;
        index2 = _chromosome[i+1] - 1;
        for (int j = 0; j < n_dim; j++) {
            distance += pow(city_coordinates[index1][j] - city_coordinates[index2][j], 2);
        }
        fitness += sqrt(distance);
        distance = 0.;
    }
    for (int j = 0; j < n_dim; j++) {
        distance += pow(city_coordinates[_chromosome[_n_genes-1]-1][j] - city_coordinates[0][j], 2);
    }

    _fitness = fitness + sqrt(distance);
}

void Individual :: pair_permutation(int a, int b) {
    if (a<0 || b<0 || a>=_n_genes || b>=_n_genes) throw invalid_argument("Error: permutation index not allowed");
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

void Individual :: gene_shift(int shift_length, int n_contiguous_genes, int start) {
    if (n_contiguous_genes >= _n_genes-1) throw invalid_argument("Error: Too many genes to shift");

    for (int i = 0; i < n_contiguous_genes; i++) {
        for (int j = 0; j < shift_length; j++) this->pair_permutation(start+i+j, start+i+j+1);
    }
}

void Individual :: multiple_permutation(int start, int end, int permutation_length) {
    if (_n_genes - end < permutation_length) throw invalid_argument("Error: Not enough genes to complete the multiple permutation");

    for (int i = 0; i < permutation_length; i++) this->pair_permutation(start+i, end+i);
}

void Individual :: inversion(int m) {
    if (m > _n_genes-1) throw invalid_argument("Error: The number of genes to invert is bigger than the chromosome");

    for (int i = 1; i <= (m-1)/2; i++) this->pair_permutation(i, m+1-i);
}