#ifndef AnalogFilterConfig_H
#define AnalogFilterConfig_H

//#define ENABLE_DEBUG

#define SIZE_OF_DATA_BUFFER 10  //  [N] size of data buffer (the amount of samples we can store)
#define MIN_SAMPLE_TIME 10      //  [ms] minimum sample time (f = 1/T, T = 10ms => f = 100 Hz)
#define MAX_SAMPLE_TIME 1000 //  [ms] maximum sample time (f = 1/T, T = 1s => f = 1 Hz)
#define MIN_SAMPLE_CNT 2        //  [N] minimum amount of samples to take
#define MAX_SAMPLE_CNT SIZE_OF_DATA_BUFFER  //  maximum amount of samples to take

#endif