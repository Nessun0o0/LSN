#include "random_walk.h"
#include <iostream>

// Generates a new step from the position given as argument for a RW on a cubic lattice
vector<double> RW_step_lattice(vector<double> pos, Random& rnd) {
    int direction = static_cast<int>(rnd.Rannyu(0.,3.));
    int step = 1 - 2 * static_cast<int>(rnd.Rannyu(0.,2.)); // Can be -1 or +1 for forward or backward step
    pos[direction] += static_cast<double>(step);
    return pos;
}

// Generates a new step from the position given as argument for a RW continuous in 3D
vector<double> RW_step_continuum(vector<double> pos, Random& rnd) {
    // Generates a uniform direction in spherical coordinates
    double phi = rnd.Rannyu(0., 2*M_PI);
    double theta = acos(1-2*rnd.Rannyu());

    pos[0] += sin(theta)*cos(phi);
    pos[1] += sin(theta)*sin(phi);
    pos[2] += cos(theta);
    return pos;
}

// Returns the squared distance of the RW to the origin
double compute_distance(vector<double> pos) {
    double distance = 0.;
    for (double coord : pos) {
        distance += pow(coord, 2);
    }
    return distance;
}