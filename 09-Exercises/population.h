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
        Population(double n_individuals, int n_cities, const vector<vector<double>>& city_coordinates, Random* rnd);

        void set_starting_population();
        void order_fitness();

        const vector<Individual>& get_population() {return _population;};
        void check_population_constraints();
        
        vector<Individual> selection();
    private:
        int _n_cities, _n_individuals;
        vector<vector<double>> _city_coordinates;
        vector<Individual> _population;
        Random* _rnd;
};

#endif