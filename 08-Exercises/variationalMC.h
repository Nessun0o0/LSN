#ifndef __variationalMC__
#define __variationalMC__

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "random.h"

using namespace std;

// Class used to compute the expectation energy of the quantum state with the Metropolis algorithm
class ExpectationEnergy {
    public:
        ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd);

        void set_x(double x) {_x = x;};
        void set_mu(double mu) {_mu = mu;};
        void set_sigma(double sigma) {_sigma = sigma;};
        void set_delta(double delta) {_delta = delta;};

        void metro();
        void compute_energy_blocking(int n_blocks, int n_steps);
        double compute_energy(int n_steps);

        double psi_T(double x);
        double H_psi_T();
        
        double get_acceptance();
        double get_H() {return _H;};
        double get_error() {return _err;}; 
        double get_x() {return _x;};
    private:
        double _x, _mu, _sigma, _delta;
        double _H, _err;
        int _nblocks, _nsteps;
        int _n_accepted, _n_tries;
        Random* _rnd;
};

// Class that implements the simulating annealing algorithm to evolve
class SimulatedAnnealing {
    public:
        SimulatedAnnealing(double mu, double sigma, double T, double delta, Random* rnd);
    
        void set_temp(double temp) {_temp = temp; _beta = 1./temp;};
        void set_delta_H(double delta) {_var_H.set_delta(delta);};
        void set_delta_SA(double delta) {_delta = delta;};
        void set_compute_H_steps(int H_steps) {_compute_H_steps = H_steps;};
        void metro();
        void reset_acceptance() {_n_accepted = 0, _n_tries = 0;};
        void start_annealing();
        void annealing_step(double tries);

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
        int _n_accepted, _n_tries, _compute_H_steps, _SA_step;
        ExpectationEnergy _var_H;
        double _current_H;
        Random* _rnd;
};

#endif
