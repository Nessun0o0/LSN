#include "random_walk.h"
#include <iostream>

double RW_distance_lattice(int steps, Random& rnd) {
    std::vector<double> pos={0., 0., 0.};

    for (int i = 0; i < steps; i++) {
        int direction = static_cast<int>(rnd.Rannyu(0.,3.));
        int step = 1 - 2 * static_cast<int>(rnd.Rannyu(0.,2.));
        pos[direction] += static_cast<double>(step);
    }
    return std::pow(pos[0], 2) + std::pow(pos[1], 2) + std::pow(pos[2], 2);
}



double RW_distance_continuum(int steps, Random& rnd) {
    std::vector<double> pos{0., 0., 0.}; // x, y, z

    for (int i = 0; i < steps; i++) {
        double phi = rnd.Rannyu(0., 2*M_PI);
        double theta = std::acos(1-2*rnd.Rannyu());

        pos[0] += std::sin(theta)*std::cos(phi);
        pos[1] += std::sin(theta)*std::sin(phi);
        pos[2] += std::cos(theta);
    }
    return std::pow(pos[0], 2) + std::pow(pos[1], 2) + std::pow(pos[2], 2);
}

std::vector<double> RW_step_lattice(std::vector<double> pos, Random& rnd) {
    int direction = static_cast<int>(rnd.Rannyu(0.,3.));
    int step = 1 - 2 * static_cast<int>(rnd.Rannyu(0.,2.));
    pos[direction] += static_cast<double>(step);
    return pos;
}

double compute_distance(std::vector<double> pos) {
    double distance = 0.;
    for (double coord : pos) {
        distance += std::pow(coord, 2);
    }
    return distance;
}