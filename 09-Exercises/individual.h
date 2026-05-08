#ifndef __individual__
#define __individual__

#include <vector>
#include <cmath>

using namespace std;

class Individual {
    public:
        Individual() : _chromosome(0) {_n_genes=0;};
        Individual(vector<int> chromosome);

        void compute_fitness(const vector<vector<double>>& city_coordinates);
        void pair_permutation(int a, int b);

        const vector<int>& get_chromosome() {return _chromosome;};
        double get_fitness() {return _fitness;};

        bool check_constraints();

    private:
        vector<int> _chromosome;
        int _n_genes;
        double _fitness;
};

#endif