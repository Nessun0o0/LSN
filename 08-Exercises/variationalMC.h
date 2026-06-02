#ifndef __variationalMC__
#define __variationalMC__

#include <cmath>
#include <string>
#include <iostream>
#include "random.h"

using namespace std;

class ExpectationEnergy {
    public:
        ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd);

        void set_x(double x) {_x = x;};
        void set_mu(double mu) {_mu = mu;};
        void set_sigma(double sigma) {_sigma = sigma;};
        void set_delta(double delta) {_delta = delta;};

        void metro();

        double psi_T(double x);
        double H_psi_T();
        double get_acceptance();
        double compute_energy(int n_steps);
    private:
        double _x, _mu, _sigma;
        double _delta;
        int _n_accepted, _n_tries;
        Random* _rnd;
};

class SimulatedAnnealing {
    public:
        SimulatedAnnealing(double mu, double sigma, double T, double delta, Random* rnd);
    
        void set_temp(double temp) {_temp = temp; _beta = 1./temp;};
        void set_delta_H(double delta) {_var_H.set_delta(delta);};
        void set_delta_SA(double delta) {_delta = delta;};
        void metro();
        void reset_acceptance() {_n_accepted = 0, _n_tries = 0;};

        double get_acceptance() {return static_cast<double>(_n_accepted)/_n_tries;};
        double get_H_acceptance() {return _var_H.get_acceptance();};
        double get_mu() {return _mu;};
        double get_sigma() {return _sigma;};
        double get_delta() {return _delta;};
        double get_tries() {return _n_tries;};
        double get_H() {return _current_H;};
    private:
        double _mu, _sigma, _delta;
        double _temp, _beta;
        int _n_accepted, _n_tries, _compute_H_steps;
        ExpectationEnergy _var_H;
        double _current_H;
        Random* _rnd;
};

#endif
