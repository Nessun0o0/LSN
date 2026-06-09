#include "variationalMC.h"

/*
ExpectationEnergy methods
*/


// Class constructor
ExpectationEnergy :: ExpectationEnergy(double x, double mu, double sigma, double delta, Random* rnd) {
    _x = x;
    _mu = mu;
    _sigma = sigma;
    _delta = delta;
    _rnd = rnd;
    _n_accepted = 0;
    _n_tries = 0;
}

// Computes the value of the wave function
double ExpectationEnergy :: psi_T(double x) {
    return exp(-pow(x-_mu,2) / (2.*_sigma*_sigma)) + exp(-pow(x+_mu,2) / (2.*_sigma*_sigma));
}

// Computes the value of (H psi_T) / psi_T
double ExpectationEnergy :: H_psi_T() {
    return (1 - (_x*_x + _mu*_mu - 2*_mu*_x*tanh(_x*_mu/pow(_sigma,2)))/pow(_sigma,2))/(2.*pow(_sigma,2)) + pow(_x,4) - 2.5*pow(_x,2);
}

// Perform a Metropolis step
void ExpectationEnergy :: metro() {
    double x_new = _rnd->Rannyu(-1.,1.)*_delta + _x;
    double acceptance = pow(this->psi_T(x_new) / this->psi_T(_x), 2);
    if (_rnd->Rannyu() < acceptance) {
        _x = x_new;
        _n_accepted++;
    }
    _n_tries++;
}

// Return the acceptance rate of the Metropolis moves
double ExpectationEnergy :: get_acceptance() {
    return static_cast<double>(_n_accepted) / _n_tries;
}

// Computes the energy as the average of n_steps steps. Does not compute statistical uncertainties
double ExpectationEnergy :: compute_energy(int n_steps) {
    _n_accepted = 0;
    _n_tries = 0;
    double sum_H = 0.;
    for (int i = 0; i < n_steps; i++) {
        this->metro();
        sum_H += this->H_psi_T();
    }
    return sum_H / static_cast<double>(n_steps);
}

// Computes the energy with the data blocking method and saves the result in the class datamembers
void ExpectationEnergy :: compute_energy_blocking(int n_blocks, int n_steps) {
    _n_accepted = 0;
    _n_tries = 0;
    double H = 0.;
    double H_squared = 0.;
    for (int i = 0; i < n_blocks; i++) {
        double block_sum = 0.;
        for (int j = 0; j < n_steps; j++) {
            this->metro();
            block_sum += this->H_psi_T();
        }
        block_sum /= static_cast<double>(n_steps);
        H += block_sum;
        H_squared += block_sum * block_sum;
    }

    H /= static_cast<double>(n_blocks);
    H_squared /= static_cast<double>(n_blocks);

    _H = H;
    _err = sqrt((H_squared - H*H) / (n_blocks-1));
} 

/*
SimulatedAnnealing methods
*/

// SimulatedAnnealing class constructor
SimulatedAnnealing :: SimulatedAnnealing(double mu, double sigma, double T, double delta, Random* rnd) : _var_H(0., mu, sigma, 2.5*sigma, rnd) {
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

// Performs a Metropolis step to optimize the wave function parameters
void SimulatedAnnealing :: metro() {
    double mu_new = _mu +  _rnd->Rannyu(-1.,1.)*_delta;
    double sigma_new = _sigma +  _rnd->Rannyu(-1.,1.)*_delta;

    // Reject moves with negative mu and sigma for the symmetry of the problem
    // Also reject sigma that are too small to avoid problem with division with small numbers
    if (mu_new < 0. || sigma_new <= 0.1) {
        _n_tries++;
        return;
    }

    _var_H.set_mu(mu_new);
    _var_H.set_sigma(sigma_new);
    _var_H.set_delta(2.5*sigma_new);
    _var_H.set_x(0.);
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

// Prepares the output file
void SimulatedAnnealing :: start_annealing() {
    ofstream output("OUTPUT/energies_SA.out");
    if (output.is_open()) output << "#          STEP:   TEMPERATURES:             MU:          SIGMA:         ENERGY:          ERROR:\n";
    else cerr << "ERROR: cannot open OUTPUT/energies_SA.out\n";
    output.close();
    _SA_step = 1;
}

// Perform a SA step, containing `tries` metropolis moves
// After each step the energy is evaluated using data blocking with the final parameters
void SimulatedAnnealing :: annealing_step(double tries) {
    this->reset_acceptance(); // To supervise the acceptance rate of each temperature
    for (int i = 0; i < tries; i++) {
        this->metro();
    }
    
    _var_H.set_mu(_mu);
    _var_H.set_sigma(_sigma);
    _var_H.set_delta(2.5*_sigma);
    _var_H.compute_energy_blocking(100, 1000);

    ofstream output("OUTPUT/energies_SA.out", ios::app);
    output << setw(16) << _SA_step << setw(16) << _temp << setw(16) << _mu << setw(16) << _sigma
           << setw(16) << _var_H.get_H() << setw(16) << _var_H.get_error() << "\n";
    output.close();

    // Adjust the delta to have an acceptance rate ~ 0.5
    if (this->get_acceptance() > 0.55) _delta*=1.2;
    else if (this->get_acceptance() < 0.45) _delta/=1.2;

    _SA_step++;
}