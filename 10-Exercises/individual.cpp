#include "individual.h"
#include <iostream>

// Class constructor
Individual :: Individual(vector<int> chromosome) {
    _chromosome = chromosome;
    _n_genes = chromosome.size();
}

// Computes the length of the path between the cities in the order of the chromosome
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

// Checks whether the constraints on the chromosome are satisfied
bool Individual :: check_constraints() {
    if (_chromosome[0] != 1) return false;
    int sum = 1;
    for (int i = 1; i <_n_genes; i++) sum += _chromosome[i];
    return sum == (_n_genes+1)*_n_genes/2;
}

// Swaps two genes
void Individual :: pair_permutation(int a, int b) {
    if (a<0 || b<0 || a>=_n_genes || b>=_n_genes) throw invalid_argument("Error: permutation index not allowed");
    if (a == b) return;
    _chromosome[a] ^= _chromosome[b];
    _chromosome[b] ^= _chromosome[a];
    _chromosome[a] ^= _chromosome[b];
}

// Shift n_contiguous_genes genes to the right shift_length places from a given starting position
void Individual :: gene_shift(int shift_length, int n_contiguous_genes, int start) {
    if (start < 1 || n_contiguous_genes >= _n_genes-1) throw invalid_argument("Error: Shift operation out of bounds");

    int N = _n_genes-1;
    vector<int> original = _chromosome;

    for (int i = 0; i < N; i++) {
        if (i < shift_length + n_contiguous_genes) {
            if (i >= shift_length) _chromosome[1 + (start + i - 1)%N] = original[1 + (start + i - shift_length -1)%N]; // copy the shifted genes in their new position
            else _chromosome[1 + (start + i - 1)%N] = original[1 + (start + i + n_contiguous_genes -1)%N]; // shift to the left the genes surpassed by the shift
        } else {
            _chromosome[1 + (start + i - 1)%N] = original[1 + (start + i - 1)%N]; // Copy the remaining genes that are untouched
        }
    }

}

// Permutation along permutation_length contiguous cities
void Individual :: multiple_permutation(int start, int end, int permutation_length) {
    if (_n_genes - end < permutation_length) throw invalid_argument("Error: Not enough genes to complete the multiple permutation");

    for (int i = 0; i < permutation_length; i++) this->pair_permutation(start+i, end+i);
}

// Invert length genes from a given starting position
void Individual :: inversion(int start, int length) {
    if (start < 1 || start+length > _n_genes) throw invalid_argument("Error: The number of genes to invert is bigger than the chromosome");

    // Swaps the pair of genes symmetric to the center of the inversion
    for (int i = 0; i < length / 2; i++) this->pair_permutation(start + i, start + length - 1 - i);
}