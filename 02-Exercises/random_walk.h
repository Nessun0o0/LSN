#pragma once

#include <vector>
#include <cmath>
#include "random.h"

using namespace std;

// Generates a new step from the position given as argument for a RW on a cubic lattice
vector<double> RW_step_lattice(vector<double> pos, Random& rnd);
// Generates a new step from the position given as argument for a RW continuous in 3D
vector<double> RW_step_continuum(vector<double> pos, Random& rnd);

// Returns the squared distance of the RW to the origin
double compute_distance(vector<double> pos);