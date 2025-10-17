/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>
#include "pthread.h"

/*struct Args
{
    Matrix m;
    Matrix blurred;
    int radius;
    int x_max;
    int x_min;
};*/

//pthread_mutex_t blurredLock;
//pthread_mutex_t scratchLock;
pthread_cond_t waitingForOtherThreads;
pthread_mutex_t waitLock;
int threadsNotDone;

namespace Filter
{

    namespace Gauss
    {
        void get_weights(int n, double *weights_out)
        {
            for (auto i{0}; i <= n; i++)
            {
                double x{static_cast<double>(i) * max_x / n};
                weights_out[i] = exp(-x * x * pi);
            }
        }
    }

    void *blur(void *args)
    {
        ThreadData *data = (ThreadData*)args;

        double w[Gauss::max_radius]{};
        Gauss::get_weights(data->radius, w);

        for (auto x{data->x_min}; x < data->x_max; x++)
        {
            for (auto y{0}; y < data->y_max; y++)
            {
                //double w[Gauss::max_radius]{};
                //Gauss::get_weights(radius, w);

                // unsigned char Matrix::r(unsigned x, unsigned y) const
                // {
                //     return R[y * x_size + x];
                // }

                auto r{w[0] * data->m->r(x, y)}, g{w[0] * data->m->g(x, y)}, b{w[0] * data->m->b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= data->radius; wi++)
                {
                    auto wc{w[wi]};
                    auto x2{x - wi};
                    if (x2 >= 0)
                    {
                        data->m->rgb(x2, y, wc, r, g, b);
                        //r += wc * dst.r(x2, y);
                        //g += wc * dst.g(x2, y);
                        //b += wc * dst.b(x2, y);
                        n += wc;
                    }
                    x2 = x + wi;
                    if (x2 < data->m->get_x_size())
                    {
                        data->m->rgb(x2, y, wc, r, g, b);
                        //r += wc * dst.r(x2, y);
                        //g += wc * dst.g(x2, y);
                        //b += wc * dst.b(x2, y);
                        n += wc;
                    }
                }
                //pthread_mutex_lock(&scratchLock);
                data->scratch->r(x, y) = r / n;
                data->scratch->g(x, y) = g / n;
                data->scratch->b(x, y) = b / n;
                //pthread_mutex_unlock(&scratchLock);
            }
        }
        pthread_mutex_lock(&waitLock);
        threadsNotDone--;   

        if (threadsNotDone >= 1)
        {
            pthread_cond_wait(&waitingForOtherThreads, &waitLock);
        }
        else
        {
            pthread_cond_broadcast(&waitingForOtherThreads);
        }
        pthread_mutex_unlock(&waitLock);
        
        for (auto x{data->x_min}; x < data->x_max; x++)
        {
            for (auto y{0}; y < data->y_max; y++)
            {
                //double w[Gauss::max_radius]{};
                //Gauss::get_weights(radius, w);

                auto r{w[0] * data->scratch->r(x, y)}, g{w[0] * data->scratch->g(x, y)}, b{w[0] * data->scratch->b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= data->radius; wi++)
                {
                    auto wc{w[wi]};
                    auto y2{y - wi};
                    if (y2 >= 0)
                    {
                        data->scratch->rgb(x, y2, wc, r, g, b);
                        //r += wc * scratch.r(x, y2);
                        //g += wc * scratch.g(x, y2);
                        //b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                    y2 = y + wi;
                    if (y2 < data->y_max)
                    {
                        data->scratch->rgb(x, y2, wc, r, g, b);
                        //r += wc * scratch.r(x, y2);
                        //g += wc * scratch.g(x, y2);
                        //b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                }
                //pthread_mutex_lock(&blurredLock);
                data->blurred->r(x, y) = r / n;
                data->blurred->g(x, y) = g / n;
                data->blurred->b(x, y) = b / n;
                //pthread_mutex_unlock(&blurredLock);
            }
        }
    }

    
}
