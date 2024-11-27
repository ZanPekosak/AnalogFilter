#ifndef AnalogFilter_H
#define AnalogFilter_H

#include <stdint.h>
#include "AnalogFilterConfig.h"

/*Enum to determine type of filtering, if any*/
/*Some types are yet to be implemented*/
enum filterType{
        NONE,       //  enables the library to only function as way to slow down the CPU - useful when debugging (maybe)
        AVERAGE,    //  calculates the average of the last n points (generally not a useful filter, but came in handy once, so here it is)
        RUN_AVG,    //  the running average filter - returns the running average for each point and a callback for each n point
//     LOWPASS,    //  low pass filter - name says it all for the following 3
//     HIGHPASS,
//     BANDPASS,
//     BUTTERWORTH //  classic Butterworth IIR filter
    NUM_OF_ELEMENTS //  conviently store number of possible values in ENUM
};

class A_FILTER{
    public:
        int32_t dataPoint = 0;  //  public data point to access

        A_FILTER(uint8_t pin);
        A_FILTER(uint8_t pin, uint16_t sampleTime);
        A_FILTER(uint8_t pin, uint16_t sampleTime, uint8_t numSamples);
        A_FILTER(uint8_t pin, uint16_t sampleTime, uint8_t numSamples, filterType fType);
        
        /*Initializes the pin to be read and checks for any errors during constructor creation*/
        void begin();
        
        /*Put this in the beggining of loop to start sampling data*/
        void loop();
        
        /*Get the latest filtered data point*/
        int32_t getData();
        
        /*Get the latest data point (same as analogRead(), but faster in this case)*/
        int32_t getCurrentDataPoint();
        
        /*Clear the data buffer and set values to 0*/
        void clearData();

        /*Left for the user of the library to create a custom implementation if needed*/
        void sampleCountCpltCallback(uint8_t pin, int32_t filteredData)__attribute__((weak));

    private:
        /*Enum to store if something went wrong in the constructor*/
        enum constructorError{
            BLANK,
            MINSAMPLE,
            MAXSAMPLE,
            MINSAMPTIME,
            MAXSAMPTIME,
            ENUM_NOT_CORRECT
        };

        constructorError _err = BLANK;

        uint8_t _pin = 0;                   //  chosen pin
        uint16_t _sampleTime = 0;           //  chosen sample time
        uint8_t _numSamples = 0;            //  variable to keep track of number of samples
        int32_t _data[SIZE_OF_DATA_BUFFER]; //  buffer for storing the data
        int32_t _fData = 0;                 //  current filtered datapoint
        uint8_t _readCounter = 0;           //  tracks how many data points have been read
        uint8_t _writeCounter = 0;          //  tracks how many data points have been written
        uint8_t _iterationCounter = 0;      //  tracks how many samples have elapsed
        uint32_t _timeTrack = 0;            //  tracks the elapsed time from the last sampling

        /*Read value from ADC and store it in out array*/
        void sampleAndStore();

        /*Calculate the average of the last n-th data points*/
        int32_t calcAvg();

        /*Calculate the running (moving) average of the last n-th data points with 2 iteration steps between points*/
        int32_t calcRunAvg();

        /*Check if there is any data available in the buffer*/
        bool isDataAvailable();

        /*Clear information about data availability and skip last unprocessed data points*/
        void clearDataAvailable();

        /*Increments the write counter with automatic rollover*/
        uint8_t incrementWriteCounter();
        
        /*Increments the read counter with automatic rollower*/
        uint8_t incrementReadCounter();

        /*Increments the iteration counter - used to match the other increments*/
        uint8_t incrementIterationCounter();
};

#endif