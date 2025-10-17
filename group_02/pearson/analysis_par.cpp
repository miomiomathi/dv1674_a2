/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis_par.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>

namespace Analysis {

void* correlation_coefficients(void* arg)
{
    auto thread_data = static_cast<ThreadData*>(arg);
    auto& datasets = *thread_data->datasets;
    std::vector<double>& results = thread_data->results;

    // std::vector<double> result {};

    int size_dataset = static_cast<int>(datasets.size());
    int start = thread_data->start;
    int end = thread_data->end;
    int k = 0;

    for (auto sample1 { 0 }; sample1 < size_dataset - 1; sample1++) {
        for (auto sample2 { sample1 + 1 }; sample2 < size_dataset; sample2++) {
            if(k >= start && k < end){
                auto corr { pearson(datasets[sample1], datasets[sample2]) };
                results.push_back(corr);
            }
            ++k; if(k >= end) break;
        }
        if(k >= end) break;
    }

    pthread_exit(nullptr);
    // return result;
}

//add thread function 

double pearson(Vector& vec1, Vector& vec2)
{
    // auto x_mean { vec1.mean() };
    // auto y_mean { vec2.mean() };

    // auto x_mm { vec1 - x_mean };
    // auto y_mm { vec2 - y_mean };

    // auto x_mag { x_mm.magnitude() };
    // auto y_mag { y_mm.magnitude() };

    // auto x_mm_over_x_mag { x_mm / x_mag };
    // auto y_mm_over_y_mag { y_mm / y_mag };

    // auto r { x_mm_over_x_mag.dot(y_mm_over_y_mag) };
    auto r { vec1.dot(vec2) };

    return std::max(std::min(r, 1.0), -1.0);
}
};
