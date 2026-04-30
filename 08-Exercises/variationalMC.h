#ifndef __variationalMC__
#define __variationalMC__

#include <cmath>
#include <string>
#include "random.h"

using namespace std;

class ExpectationEnergy {
    public:
        ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd);

        void set_x(double x) {_x = x;};
        void set_mu(double mu) {_mu = mu;};
        void set_sigma(double sigma) {_sigma = sigma;};

        void metro();

        double psi_T(double x);
        double H_psi_T();
        double get_acceptance();
    private:
        double _x, _mu, _sigma;
        double _delta;
        int _n_accepted, _n_tries;
        Random* _rnd;
};


#endif
