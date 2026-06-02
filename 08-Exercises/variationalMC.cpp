#include "variationalMC.h"

ExpectationEnergy :: ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd) {
    _x = x;
    _mu = mu;
    _sigma = sigma;
    _delta = delta;
    _rnd = rnd;
    _n_accepted = 0;
    _n_tries = 0;
}

double ExpectationEnergy :: psi_T(double x) {
    return exp(-pow(x-_mu,2) / (2.*_sigma*_sigma)) + exp(-pow(x+_mu,2) / (2.*_sigma*_sigma));
}

double ExpectationEnergy :: H_psi_T() {
    return (1 - (_x*_x + _mu*_mu - 2*_mu*_x*tanh(_x*_mu/pow(_sigma,2)))/pow(_sigma,2))/(2.*pow(_sigma,2)) + pow(_x,4) - 2.5*pow(_x,2);
}

void ExpectationEnergy :: metro() {
    double x_new = _rnd->Rannyu(-1.,1.)*_delta + _x;
    double acceptance = pow(this->psi_T(x_new) / this->psi_T(_x), 2);
    if (_rnd->Rannyu() < acceptance) {
        _x = x_new;
        _n_accepted++;
    }
    _n_tries++;
}

double ExpectationEnergy :: get_acceptance() {
    return static_cast<double>(_n_accepted) / _n_tries;
}

double ExpectationEnergy :: compute_energy(int n_steps) {
    // Thermalization: finds the region where the new wave function is relevant
    for (int i = 0; i < 500; i++) this->metro();

    _n_accepted = 0;
    _n_tries = 0;
    double sum_H = 0.;
    for (int i = 0; i < n_steps; i++) {
        this->metro();
        sum_H += this->H_psi_T();
    }
    return sum_H / static_cast<double>(n_steps);
}

// SIMULATED ANNEALING

SimulatedAnnealing :: SimulatedAnnealing(double mu, double sigma, double T, double delta, Random* rnd) : _var_H(0., mu, sigma, delta, rnd) {
    _mu = mu;
    _sigma = sigma;
    _delta = delta;
    this->set_temp(T);
    _rnd = rnd;
    _compute_H_steps = 10000;
    _current_H = _var_H.compute_energy(_compute_H_steps);
    _n_accepted = 0;
    _n_tries = 0;
}

void SimulatedAnnealing :: metro() {
    double mu_new = _mu;
    double sigma_new = _sigma;
    if (_rnd->Rannyu() < 0.5) mu_new += _rnd->Rannyu(-1.,1.)*_delta;
    else sigma_new += _rnd->Rannyu(-1.,1.)*_delta;

    if (mu_new < 0. || sigma_new <= 0.1) {
        _n_tries++;
        return;
    }
    _var_H.set_mu(mu_new);
    _var_H.set_sigma(sigma_new);
    _var_H.set_delta(2.*sigma_new);
    double H_new = _var_H.compute_energy(_compute_H_steps);

    double acceptance = exp(_beta * (_current_H - H_new));
    if (_rnd->Rannyu() < acceptance) {
        _mu = mu_new;
        _sigma = sigma_new;
        _current_H = H_new;
        _n_accepted++;
    }
    _n_tries++;
}
