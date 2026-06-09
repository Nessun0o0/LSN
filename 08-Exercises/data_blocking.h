#ifndef __data_blocking__
#define __data_blocking__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

// Computes cumulative averages and errors with the data blocking technique
// The cumulative averages are inserted in the vector data_blocks, the errors in the vector errors
void averages_from_blocks(vector<double>& data_blocks, const vector<double>& data_blocks_squared, vector<double>& errors) {
    int N = data_blocks.size();
    vector<double> cumsum(N), cumsum_squared(N);

    cumsum[0] = data_blocks[0];
    cumsum_squared[0] = data_blocks_squared[0];
    errors[0] = 0.;
    for (int i = 1; i < N; i++) {
        cumsum[i] = cumsum[i-1] + data_blocks[i];
        cumsum_squared[i] = cumsum_squared[i-1] + data_blocks_squared[i];

        data_blocks[i] = cumsum[i] / static_cast<double>(i+1);
        errors[i] = sqrt((cumsum_squared[i]/(i+1) - pow(data_blocks[i],2))/i);
    }
}

// Writes the cumulative averages and errors on file
int write_data(const vector<double>& averages, const vector<double>& errors, string filename){

    ofstream outfile;
    outfile.open(filename);
    if (outfile.is_open()) {
        outfile << "#  BLOCK:       AVERAGES:         ERRORS:\n";
        for (int i = 0; i < int(averages.size()); i++) {
            outfile << setw(8) << " " << i+1 << setw(15) << averages[i] << " " << setw(15) << errors[i] << endl;
        }
    } else {
        cerr << "Error: unable to write " + filename << endl;
        return -1;
    }
    outfile.close();
    return 0;
}

#endif