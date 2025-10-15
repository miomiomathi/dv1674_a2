/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <vector>

#if !defined(ANALYSIS_HPP)
#define ANALYSIS_HPP
#endif

namespace Analysis {
// std::vector<double> correlation_coefficients(std::vector<Vector>& datasets);
void* correlation_coefficients(void* thread_data);
double pearson(Vector& vec1, Vector& vec2);

struct ThreadData{
    std::vector<Vector>* datasets;
    int start;
    int end;
    std::vector<double> results;
};
};


