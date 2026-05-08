#include "population.h"

Population :: Population (double n_individuals, int n_cities, const vector<vector<double>>& city_coordinates, Random* rnd) {
    _n_cities = n_cities;
    _city_coordinates = city_coordinates;
    _n_individuals = n_individuals;
    _rnd = rnd;
    this->set_starting_population();
    check_population_constraints();
    order_fitness();
}

void Population :: set_starting_population() {
    vector<int> ordered_cities(_n_cities);
    for (int i = 0; i < _n_cities; i++) {
        ordered_cities[i] = i+1;
    }
    _population = vector<Individual>(_n_individuals, ordered_cities);
    for (int i = 0; i < _n_individuals; i++) {
        int n_shuffles = static_cast<int>(_rnd->Rannyu(0.,_n_cities));
        for (int j = 0; j < n_shuffles; j++) {
            int a = static_cast<int>(_rnd->Rannyu(1.,_n_cities));
            int b = static_cast<int>(_rnd->Rannyu(1.,_n_cities));
            _population[i].pair_permutation(a, b);
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
    return new_population;
}