#include "variationalMC.h"

ExpectationEnergy :: ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd) {
    _x = x;
    _mu = mu;
    _sigma = sigma;
    _delta = delta;
    _rnd = rnd;
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