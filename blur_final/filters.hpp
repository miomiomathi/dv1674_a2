/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "matrix.hpp"
#include "pthread.h"

#if !defined(FILTERS_HPP)
#define FILTERS_HPP

//extern pthread_mutex_t blurredLock;
//extern pthread_mutex_t scratchLock;
//extern pthread_cond_t waitingForOtherThreads;
//extern pthread_mutex_t waitLock;
//extern int threadsNotDone;

extern pthread_barrier_t barrier;

namespace Filter
{

    namespace Gauss
    {
        constexpr unsigned max_radius{1000};
        constexpr float max_x{1.33};
        constexpr float pi{3.14159};

        void get_weights(int n, double *weights_out);
    }

    Matrix blur(Matrix m, const int radius);
    void *blur(void *args);

    struct ThreadData
    {
        Matrix *m;
        Matrix *blurred;
        Matrix *scratch;
        int radius;
        int x_max;
        int x_min;
        int y_max;
    };
    

}

#endif