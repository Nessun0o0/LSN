#pragma once

#include <vector>
#include <cmath>
#include "random.h"

double RW_distance_lattice(int steps, Random& rnd);
double RW_distance_continuum(int steps, Random& rnd);

std::vector<double> RW_step_lattice(std::vector<double> pos, Random& rnd);
std::vector<double> RW_step_continuum(std::vector<double> pos, Random& rnd);

double compute_distance(std::vector<double> pos);