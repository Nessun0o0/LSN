#ifndef __population__
#define __population__

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

#include "random.h"
#include "individual.h"

class Population {
    public:
        Population(double n_individuals, int n_cities, const vector<vector<double>>& city_coordinates, double temperature, Random* rnd);

        void set_starting_population();
        void order_fitness();

        vector<Individual>& get_population() {return _population;};
        Individual get_elite() {return _elite;};
        void check_population_constraints();
        
        vector<Individual> selection();
        void evolve_population();
        void metropolis_move();
        void exchange_population(Population& target) {;};
        Individual crossover(const Individual& mother, const Individual& father, int cut);

        double get_beta() {return _beta;};
    private:
        int _n_cities, _n_individuals;
        vector<vector<double>> _city_coordinates;
        vector<Individual> _population;
        vector<double> _mutation_probs;
        double _crossover_prob, _selection_exponent;
        Individual _elite;
        double _temperature, _beta;
        int _n_accepted, _n_moves;
        Random* _rnd;
};

#endif