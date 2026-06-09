#include "population.h"
#include <iostream>

Population :: Population (double n_individuals, int n_cities, const vector<vector<double>>& city_coordinates, Random* rnd) {
    _n_cities = n_cities;
    _city_coordinates = city_coordinates;
    _n_individuals = n_individuals;
    _rnd = rnd;
    _mutation_probs = {0.1,0.1,0.1,0.1}; // [pair permutation, gene shift, multiple permutation, inversion]
    _crossover_prob = 0.7;
    _selection_exponent = 2;
    this->set_starting_population();
    this->check_population_constraints();
    this->order_fitness();
    _elite = _population[0];
}

// Generates a random starting population
void Population :: set_starting_population() {
    vector<int> ordered_cities(_n_cities);
    // Generate vector containing all the cities
    for (int i = 0; i < _n_cities; i++) {
        ordered_cities[i] = i+1;
    }
    // Shuffle the chromosome of every individual
    _population = vector<Individual>(_n_individuals, ordered_cities);
    for (int i = 0; i < _n_individuals; i++) {
        for (int j = 1; j < _n_cities; j++) {
            int a = static_cast<int>(_rnd->Rannyu(static_cast<double>(j),_n_cities));
            _population[i].pair_permutation(j, a);
        }
    }
}

// Checks whether all the population satisfies the constrains
void Population :: check_population_constraints() {
    for (int i = 0; i < _n_individuals; i++) {
        if (!_population[i].check_constraints()) throw std::runtime_error("One individual doesn't fulfill the bonds"); 
    }
}

// Sorts the population in order of increasing fitness
void Population :: order_fitness() {
    for (int i = 0; i < _n_individuals; i++) {
        _population[i].compute_fitness(_city_coordinates);
    }
    sort(_population.begin(), _population.end(), [](Individual a, Individual b) {return a.get_fitness() < b.get_fitness();});
}

// Select a new generation of individuals
vector<Individual> Population :: selection() {
    vector<Individual> new_population(_n_individuals);

    for (int i = 0; i < _n_individuals; i++) {
        int j = static_cast<int>(_n_individuals*pow(_rnd->Rannyu(), _selection_exponent));
        if (_rnd->Rannyu() >= _crossover_prob || i == _n_individuals-1) {
            new_population[i] = _population[j];
            continue;
        }
        // select a different individual and the cutoff gene with which to execute crossover
        int k;
        do {
            k = static_cast<int>(_n_individuals*pow(_rnd->Rannyu(), _selection_exponent));
        } while (j == k);
        int cutoff = static_cast<int>(_rnd->Rannyu()*(_n_cities-3))+2;
        // Apply crossover on both individuals
        new_population[i] = this->crossover(_population[j], _population[k], cutoff);
        new_population[i+1] = this->crossover(_population[k], _population[j], cutoff);
        i++;
    }

    return new_population;
}

void Population :: evolve_population() {
    vector<Individual> new_population = this->selection();

    // Cycles over all the population and applies mutations with the probabilities in _mutation_probs
    for (Individual& individual : new_population) {
        if (_rnd->Rannyu() < _mutation_probs[0]) {
            int a = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
            int b = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
            individual.pair_permutation(a, b);
        }
        if (_rnd->Rannyu() < _mutation_probs[1]) {
            int contiguous_genes = static_cast<int>(_rnd->Rannyu()*(_n_cities-2))+1;
            int shift_length = static_cast<int>(_rnd->Rannyu()*(_n_cities - 2 - contiguous_genes))+1;
            int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
            individual.gene_shift(shift_length, contiguous_genes, start);
        }
        if (_rnd->Rannyu() < _mutation_probs[2]) {
            int n = static_cast<int>(_rnd->Rannyu()*((_n_cities-1)/2))+1;
            int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-2*n))+1;
            int end = start + n + static_cast<int>(_rnd->Rannyu()*(_n_cities-2*n-start+1));
            individual.multiple_permutation(start, end, n);
        }
        if (_rnd->Rannyu() < _mutation_probs[3]) {
            int length = static_cast<int>(_rnd->Rannyu() * (_n_cities - 2)) + 2;
            int start = static_cast<int>(_rnd->Rannyu() * (_n_cities - length)) + 1;
            individual.inversion(start, length);
        }
    }

    _population = new_population;
    this->check_population_constraints();
    this->order_fitness();
    // If there is a new best solution save the best individual
    if (_population[0].get_fitness() < _elite.get_fitness()) _elite = _population[0];
}

// Returns the child of the crossover between two individuals
Individual Population :: crossover(const Individual& mother, const Individual& father, int cut) {
    Individual child = mother;
    int substituted = 0;

    // Reorders the child's tail (after the cut) by placing remaining cities in the exact sequence they appear in the father.
    for (int i = 0; i < _n_cities; i++) {
        int city = father.get_chromosome()[i];
        for (int j = cut+substituted; j < _n_cities; j++) {
            if (child.get_chromosome()[j] == city) { 
                child.pair_permutation(cut+substituted, j);
                if (++substituted == _n_cities - cut) return child;
                break;
            }
        }
    }

    return child;
}