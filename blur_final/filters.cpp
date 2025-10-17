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
//pthread_cond_t waitingForOtherThreads;
//pthread_mutex_t waitLock;
//int threadsNotDone;

pthread_barrier_t barrier;

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
        //std::cout << "thread start" << std::endl;

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

        pthread_barrier_wait(&barrier);

        
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

        //std::cout << "thread end" << std::endl;

    }

    Matrix blur(Matrix m, const int radius)
    {
        Matrix scratch{PPM::max_dimension};
        auto dst{m};

        double w[Gauss::max_radius]{};
        Gauss::get_weights(radius, w);

        for (auto x{0}; x < dst.get_x_size(); x++)
        {
            for (auto y{0}; y < dst.get_y_size(); y++)
            {
                //double w[Gauss::max_radius]{};
                //Gauss::get_weights(radius, w);

                // unsigned char Matrix::r(unsigned x, unsigned y) const
                // {
                //     return R[y * x_size + x];
                // }

                auto r{w[0] * dst.r(x, y)}, g{w[0] * dst.g(x, y)}, b{w[0] * dst.b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= radius; wi++)
                {
                    auto wc{w[wi]};
                    auto x2{x - wi};
                    if (x2 >= 0)
                    {
                        dst.rgb(x2, y, wc, r, g, b);
                        //r += wc * dst.r(x2, y);
                        //g += wc * dst.g(x2, y);
                        //b += wc * dst.b(x2, y);
                        n += wc;
                    }
                    x2 = x + wi;
                    if (x2 < dst.get_x_size())
                    {
                        dst.rgb(x2, y, wc, r, g, b);
                        //r += wc * dst.r(x2, y);
                        //g += wc * dst.g(x2, y);
                        //b += wc * dst.b(x2, y);
                        n += wc;
                    }
                }
                scratch.r(x, y) = r / n;
                scratch.g(x, y) = g / n;
                scratch.b(x, y) = b / n;
            }
        }

        for (auto x{0}; x < dst.get_x_size(); x++)
        {
            for (auto y{0}; y < dst.get_y_size(); y++)
            {
                //double w[Gauss::max_radius]{};
                //Gauss::get_weights(radius, w);

                auto r{w[0] * scratch.r(x, y)}, g{w[0] * scratch.g(x, y)}, b{w[0] * scratch.b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= radius; wi++)
                {
                    auto wc{w[wi]};
                    auto y2{y - wi};
                    if (y2 >= 0)
                    {
                        scratch.rgb(x, y2, wc, r, g, b);
                        //r += wc * scratch.r(x, y2);
                        //g += wc * scratch.g(x, y2);
                        //b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                    y2 = y + wi;
                    if (y2 < dst.get_y_size())
                    {
                        scratch.rgb(x, y2, wc, r, g, b);
                        //r += wc * scratch.r(x, y2);
                        //g += wc * scratch.g(x, y2);
                        //b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                }
                dst.r(x, y) = r / n;
                dst.g(x, y) = g / n;
                dst.b(x, y) = b / n;
            }
        }

        return dst;
    }
    
}
