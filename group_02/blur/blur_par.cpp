/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "matrix.hpp"
#include "ppm.hpp"
#include "filters.hpp"
#include <cstdlib>
#include <iostream>
#include "pthread.h"

/*struct Args
{
    Matrix m;
    Matrix blurred;
    int radius;
    int x_max;
    int x_min;
};*/

/*
pthread_mutex_t blurredLock;
pthread_mutex_t scratchLock;
pthread_cond_t waitingForOtherThreads;
pthread_mutex_t waitLock;
int threadsNotDone;
*/

int main(int argc, char const* argv[])
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " [radius] [infile] [outfile] [thread amount]" << std::endl;
        std::exit(1);
    }

    PPM::Reader reader {};
    PPM::Writer writer {};

    

    auto m { reader(argv[2]) };
    auto radius { static_cast<unsigned>(std::stoul(argv[1])) }; 
    int nrOfThreads = atoi(argv[4]);
    auto blurred = m;
    Matrix scratch {PPM::max_dimension};

    //pthread_mutex_init(&blurredLock, NULL);
    //pthread_mutex_init(&scratchLock, NULL);
    //pthread_cond_init(&waitingForOtherThreads, NULL);
    //pthread_mutex_init(&waitLock, NULL);
    pthread_barrier_init(&barrier, NULL, nrOfThreads);

    pthread_t threads[nrOfThreads];
    Filter::ThreadData data[nrOfThreads];
    //threadsNotDone = nrOfThreads;

    if(nrOfThreads <= 0){
        nrOfThreads = 1;
    }

    int mSizeX = m.get_x_size();
    int base = mSizeX / nrOfThreads;
    int rem = mSizeX % nrOfThreads;
    int y_max = m.get_y_size();

    int start;
    int add;
    int end;
    int next_start = 0;
    for (int i = 0; i < nrOfThreads; i++)
    {
        start = next_start;
        add = base + (i < rem ? 1 : 0);
        end = start + add;
        next_start = end;

        data[i].radius = radius;
        data[i].m = &m;
        data[i].x_min = start;
        data[i].x_max = end;
        data[i].y_max = y_max;
        data[i].blurred = &blurred;
        data[i].scratch = &scratch;

        pthread_create(&threads[i], NULL, Filter::blur, (void*)&data[i]);
        //std::cout << "thread start: " << i << std::endl;
    }
    for (int i = 0; i < nrOfThreads; i++)
    {
        pthread_join(threads[i], nullptr);
        //std::cout << "thread end: " << i << std::endl;

    }
    
    
    writer(blurred, argv[3]);

    pthread_barrier_destroy(&barrier);


    return 0;
}