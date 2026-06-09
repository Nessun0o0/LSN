#ifndef __individual__
#define __individual__

#include <vector>
#include <cmath>
#include <stdexcept>

using namespace std;

class Individual {
    public:
        Individual() : _chromosome(0) {_n_genes=0;};
        Individual(vector<int> chromosome);

        void compute_fitness(const vector<vector<double>>& city_coordinates);
        void pair_permutation(int a, int b);
        void gene_shift(int shift_length, int n_contiguous_genes, int start);
        void multiple_permutation(int start, int end, int shift_length);
        void inversion(int start, int length);

        const vector<int>& get_chromosome() const {return _chromosome;};
        double get_fitness() const {return _fitness;};

        bool check_constraints();

    private:
        vector<int> _chromosome;
        int _n_genes;
        double _fitness;
};

#endif