#ifndef AnalogFilter_H
#define AnalogFilter_H

#include <stdint.h>

class A_FILTER{
    public:
        A_FILTER(uint8_t pin, uint16t sampleTime, uint8_t numSamples);
        
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
        __attribute__((weak)) void sampleCountCpltCallback(int32_t filteredData);

    private:
        uint8_t _pin = 0;                   //  chosen pin
        uint16_t _sampleTime = 0;           //  chosen sample time
        uint8_t _numSamples = 0;            //  variable to keep track of number of samples
        int32_t _data[SIZE_OF_DATA_BUFFER]; //  buffer for storing the data
        int32_t _fData = 0;                 //  current filtered datapoint
        uint8_t _readCounter = 0;           //  tracks how many data points have been read
        uint8_t _writeCounter = 0;          //  tracks how many data points have been written
        uint8_t _iterationCounter = 0;      //  tracks how many samples have elapsed
        uint32_t _timeTrack = 0;            //  tracks the elapsed time from the last sampling

        /*Enum to store if something went wrong in the constructor*/
        enum constructorError{
            BLANK,
            MINSAMPLE,
            MAXSAMPLE,
            MINSAMPTIME,
            MAXSAMPTIME
        };

        // /*Enum to determine type of filtering, if any*/
        // /*Some types are yet to be implemented*/
        // enum filterType{
        //     NONE,       //  enables the library to only function as way to slow down the CPU - useful when debugging (maybe)
        //     AVERAGE,    //  calculates the average of the last n points (generally not a useful filter, but came in handy once, so here it is)
        //     RUN_AVG,    //  the running average filter - returns the running average for each point and a callback for each n point
        //     LOWPASS,    //  low pass filter - name says it all for the following 3
        //     HIGHPASS,
        //     BANDPASS,
        //     BUTTERWORTH //  classic Butterworth IIR filter
        // };

        /*Read value from ADC and store it in out array*/
        void sampleAndStore();

        /*Check if there is any data available in the buffer*/
        bool isDataAvailable();

        /*Increments the write counter with automatic rollover*/
        void incrementWriteCounter();
        
        /*Increments the read counter with automatic rollower*/
        void incrementReadCounter();

        /*Increments the iteration counter - used to match the other increments*/
        void incrementIterationCounter();
};

#endif