/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis_par.hpp"
#include "dataset.hpp"
#include <iostream>
#include <cstdlib>
#include <pthread.h>

// struct ThreadData{
//     std::vector<Vector>* datasets;
//     int start;
//     int end;
//     std::vector<double> results;
// };

using namespace std;



int main(int argc, char const* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [dataset] [outfile] [thread amount]" << std::endl;
        std::exit(1);
    }

    auto datasets { Dataset::read(argv[1]) };
    
    for (auto& vec : datasets) 
        vec.mean_normalize();

    //prepare for parallelization
    int size_dataset = datasets.size();
    // std::cout << "size datasets " << size_dataset << std::endl;
    int n_threads = atoi(argv[3]);
    // std::cout << "running with " << n_threads << std::endl; 

    if(n_threads <= 0){
        n_threads = 1;
    }
    int total_pairs = size_dataset * (size_dataset - 1)/2;
    // cout << "total pairs: " << total_pairs << endl;
    int base = total_pairs / n_threads;
    int rem = total_pairs % n_threads;
    // cout << "pairs per threads " << base << endl; 
    std::vector<pthread_t> threads(n_threads);
    std::vector<Analysis::ThreadData> thread_data(n_threads);
    int next_start = 0;
    for(int t = 0; t < n_threads; ++t){
        cout << "t " << t << endl;
        int start = next_start;
        int add = base + (t < rem ? 1 : 0);
        int end = start + add;
        next_start = end;
        // cout << "start" << start << endl;
        // cout << "end " << end << endl;
        thread_data[t] = {&datasets, start, end, {}};
        pthread_create(&threads[t], nullptr, Analysis::correlation_coefficients, &thread_data[t]);

    }

    //join threads and collect results
    std::vector<double> corrs;
    for(int t = 0; t < n_threads; ++t){
        pthread_join(threads[t], nullptr);
        corrs.insert(corrs.end(), thread_data[t].results.begin(), thread_data[t].results.end());
    }
    Dataset::write(corrs, argv[2]);

    return 0;
}
