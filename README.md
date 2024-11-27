# AnalogFilter
Arduino library used for filtering and averaging of analog signals in a similar sense to how industrial PLC's work. The library features several different methods of filtering signals and an easy to use, intuitive constructor with minimum additional code required.
 
## Class constructors
- A_FILTER(uint8_t pin);
- A_FILTER(uint8_t pin, uint16_t sampleTime);
- A_FILTER(uint8_t pin, uint16_t sampleTime, uint8_t numSamples);
- A_FILTER(uint8_t pin, uint16_t sampleTime, uint8_t numSamples, filterType fType);

Where:
- pin - analog pin to read data points from
- sampleTime - time in between reading data points
- numSamples - amount of samples to take and process before returning a value
- fType - type of filter to use

## Filter types
- NONE - no filtering applied to data - the returned value is directly passed from analogRead()
- AVERAGE - calculates and returns the last n data points captured by the library
- RUN_AVG - calculates and returns the moving average of the last n data points captured by the library. 
            Set number of samples to 1 in order to get regular moving average behavior
