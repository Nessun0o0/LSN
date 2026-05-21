#include "population.h"
#include <iostream>

Population :: Population (double n_individuals, int n_cities, const vector<vector<double>>& city_coordinates, double temperature, Random* rnd) {
    _n_cities = n_cities;
    _city_coordinates = city_coordinates;
    _n_individuals = n_individuals;
    _rnd = rnd;
    _temperature = temperature;
    _beta = 1./temperature;
    _n_accepted = 0;
    _n_moves = 0;
    _mutation_probs = {0.1,0.1,0.1,0.1};
    _crossover_prob = 0.7;
    _selection_exponent = 2;
    this->set_starting_population();
    this->check_population_constraints();
    this->order_fitness();
    _elite = _population[0];
}

void Population :: set_starting_population() {
    vector<int> ordered_cities(_n_cities);
    for (int i = 0; i < _n_cities; i++) {
        ordered_cities[i] = i+1;
    }
    _population = vector<Individual>(_n_individuals, ordered_cities);
    for (int i = 0; i < _n_individuals; i++) {
        for (int j = 1; j < _n_cities; j++) {
            int a = static_cast<int>(_rnd->Rannyu(static_cast<double>(j),_n_cities));
            _population[i].pair_permutation(j, a);
        }
    }
}

void Population :: check_population_constraints() {
    for (int i = 0; i < _n_individuals; i++) {
        if (!_population[i].check_constraints()) throw std::runtime_error("One individual doesn't fulfill the bonds"); 
    }
}

void Population :: order_fitness() {
    for (int i = 0; i < _n_individuals; i++) {
        _population[i].compute_fitness(_city_coordinates);
    }
    sort(_population.begin(), _population.end(), [](Individual a, Individual b) {return a.get_fitness() < b.get_fitness();});
}

vector<Individual> Population :: selection() {
    vector<Individual> new_population(_n_individuals);

    for (int i = 0; i < _n_individuals; i++) {
        int j = static_cast<int>(_n_individuals*pow(_rnd->Rannyu(), _selection_exponent));
        if (_rnd->Rannyu() >= _crossover_prob || i == _n_individuals-1) {
            new_population[i] = _population[j];
            continue;
        }
        int k = static_cast<int>(_n_individuals*pow(_rnd->Rannyu(), _selection_exponent));
        int cutoff = static_cast<int>(_rnd->Rannyu()*(_n_cities-3))+2;
        new_population[i] = this->crossover(_population[j], _population[k], cutoff);
        new_population[i+1] = this->crossover(_population[k], _population[j], cutoff);
        i++;
    }

    return new_population;
}

void Population :: evolve_population() {
    vector<Individual> new_population = this->selection();

    for (Individual& individual : new_population) {
        if (_rnd->Rannyu() < _mutation_probs[0]) {
            int a = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
            int b = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
            individual.pair_permutation(a, b);
        }
        if (_rnd->Rannyu() < _mutation_probs[1]) {
            int m = static_cast<int>(_rnd->Rannyu()*(_n_cities-2))+1;
            int n = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-m))+1;
            int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-m-n))+1;
            individual.gene_shift(n, m, start);
        }
        if (_rnd->Rannyu() < _mutation_probs[2]) {
            int n = static_cast<int>(_rnd->Rannyu()*((_n_cities-1)/2))+1;
            int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-n))+1;
            int end = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-n-start))+1;
            individual.multiple_permutation(start, end, n);
        }
        if (_rnd->Rannyu() < _mutation_probs[3]) {
            int m = static_cast<int>(_rnd->Rannyu()*(_n_cities-2))+2;
            individual.inversion(m);
        }
    }

    _population = new_population;
    this->check_population_constraints();
    this->order_fitness();
    if (_population[0].get_fitness() < _elite.get_fitness()) _elite = _population[0];
}

Individual Population :: crossover(const Individual& mother, const Individual& father, int cut) {
    Individual child = mother;
    int substituted = 0;
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

void Population :: metropolis_move() {
    Individual child = _population[0];

    int mutation_type = static_cast<int>(_rnd->Rannyu()*4);
    if (mutation_type == 0) {
        int a = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
        int b = static_cast<int>(_rnd->Rannyu()*(_n_cities-1))+1;
        child.pair_permutation(a, b);
    }
    else if (mutation_type == 1) {
        int m = static_cast<int>(_rnd->Rannyu()*(_n_cities-2))+1;
        int n = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-m))+1;
        int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-m-n))+1;
        child.gene_shift(n, m, start);
    }
    else if (mutation_type == 2) {
        int n = static_cast<int>(_rnd->Rannyu()*((_n_cities-1)/2))+1;
        int start = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-n))+1;
        int end = static_cast<int>(_rnd->Rannyu()*(_n_cities-1-n-start))+1;
        child.multiple_permutation(start, end, n);
    }
    if (mutation_type == 3) {
        int m = static_cast<int>(_rnd->Rannyu()*(_n_cities-2))+2;
        child.inversion(m);
    }

    this->check_population_constraints();
    child.compute_fitness(_city_coordinates);

    double acceptance = exp(_beta*(_population[0].get_fitness() - child.get_fitness()));
    if (_rnd->Rannyu() < acceptance) {
        _population[0] = child;
        _n_accepted++;
        if (child.get_fitness() < _elite.get_fitness()) _elite = child;
    }
    _n_moves++;
}