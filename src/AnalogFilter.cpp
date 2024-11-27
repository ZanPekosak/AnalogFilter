#include "Arduino.h"
#include "AnalogFilter.h"
#include "AnalogFilterConfig.h"

enum A_FILTER::constructorError _err = BLANK;    //  set up an enum to track if there was a constructor error
enum A_FILTER::filterType _fTyp = NONE;           //  set default filter type enum

A_FILTER::A_FILTER(uint8_t pin){
    _pin = pin;

    _sampleTime = (uint16_t)(((MAX_SAMPLE_TIME - MIN_SAMPLE_TIME)/2)+MIN_SAMPLE_TIME);  //  set the average sample time
    _numSamples = (uint8_t)(((MAX_SAMPLE_CNT - MIN_SAMPLE_CNT)/2)+MIN_SAMPLE_CNT);      //  set the average sample count

    _fTyp = AVERAGE;    //  set the default filter to averaging
}

A_FILTER::A_FILTER(uint8_t pin, uint16t sampleTime){
    _pin = pin;

    if(sampleTime < MIN_SAMPLE_TIME){
        _sampleTime = MIN_SAMPLE_TIME;  //  set the min count to the defined value in config
        _err = MINSAMPTIME;             //  set error
    }
    else if(sampleTime > MAX_SAMPLE_TIME){
        _sampleTime = MAX_SAMPLE_TIME;  //  set the max count to the defined value in config
        _err = MAXSAMPTIME;             //  set error
    }
    else{
        _sampleTime = sampleTime;
    }

    _numSamples = (uint8_t)(((MAX_SAMPLE_CNT - MIN_SAMPLE_CNT)/2)+MIN_SAMPLE_CNT);      //  set the average sample count

    _fTyp = AVERAGE;    //  set the default filter to averaging
}

A_FILTER::A_FILTER(uint8_t pin, uint16t sampleTime, uint8_t numSamples){
    _pin = pin;

    if(sampleTime < MIN_SAMPLE_TIME){
        _sampleTime = MIN_SAMPLE_TIME;  //  set the min count to the defined value in config
        _err = MINSAMPTIME;             //  set error
    }
    else if(sampleTime > MAX_SAMPLE_TIME){
        _sampleTime = MAX_SAMPLE_TIME;  //  set the max count to the defined value in config
        _err = MAXSAMPTIME;             //  set error
    }
    else{
        _sampleTime = sampleTime;
    }

    if(numSamples < MIN_SAMPLE_CNT){
        _numSamples = MIN_SAMPLE_CNT;   //  set the min count to the defined value in config
        _err = MINSAMPLE;               //  set error
    }
    else if(numSamples > MAX_SAMPLE_CNT){
        _numSamples = MAX_SAMPLE_CNT;   //  set the max count to the defined value in config
        _err = MAXSAMPLE;               //  set error
    }
    else{
        _numSamples = numSamples;
    }

    _fTyp = AVERAGE;    //  set the default filter to averaging
}

A_FILTER::A_FILTER(uint8_t pin, uint16t sampleTime, uint8_t numSamples, filterType fType){
    _pin = pin;

    if(sampleTime < MIN_SAMPLE_TIME){
        _sampleTime = MIN_SAMPLE_TIME;  //  set the min count to the defined value in config
        _err = MINSAMPTIME;             //  set error
    }
    else if(sampleTime > MAX_SAMPLE_TIME){
        _sampleTime = MAX_SAMPLE_TIME;  //  set the max count to the defined value in config
        _err = MAXSAMPTIME;             //  set error
    }
    else{
        _sampleTime = sampleTime;
    }

    if(numSamples < MIN_SAMPLE_CNT){
        _numSamples = MIN_SAMPLE_CNT;   //  set the min count to the defined value in config
        _err = MINSAMPLE;               //  set error
    }
    else if(numSamples > MAX_SAMPLE_CNT){
        _numSamples = MAX_SAMPLE_CNT;   //  set the max count to the defined value in config
        _err = MAXSAMPLE;               //  set error
    }
    else{
        _numSamples = numSamples;
    }

    if(fType >= NUM_OF_ELEMENTS){   //  if enum larger or same as number of elements marker
        _err = ENUM_NOT_CORRECT;    //  set error
        _fTyp = NONE;               //  set filter type to default - none
    }
    else{
        _fTyp = fType;  //  store enum
    }
}

void A_FILTER::begin(){
    pinMode(_pin, INPUT);

    /*Print out the warnings if any were found*/
    if(_err != BLANK){
        #ifdef ENABLE_DEBUG
            Serial.print("Warning in parameter input in class constructor A_FILTER(). ");
            Serial.print(   (err == MINSAMPLE) ? "Sample number less than configured minimum!" :
                            (err == MAXSAMPLE) ? "Sample number greater than configured maximum!" :
                            (err == MINSAMPTIME) ? "Sample time lower than configured minimum!" :
                            (err == MAXSAMPTIME) ? "Sample time greater than configured maximum!" : "Unknown error!");
            Serial.print(" In line: ");
            Serial.print(__LINE__);
            Serial.print("\n");
        #endif
    }

    _timeTrack = millis();  //  set the starting value for the sampling timer
}

void A_FILTER::loop(){
    /*If enough time has elapsed to take new sample*/
    if((millis() - _timeTrack) >= _sampleTime){
        _timeTrack = millis();  //  reset the counter
        sampleAndStore();       //  store data in buffer

        /*Only call the funtion if there is any data - it should be*/
        if(_fTyp == NONE && isDataAvailable()){
            if(_iterationCounter >= _numSamples){
                _iterationCounter = 0;              //  reset iteration counter
                _fData = _data[_writeCounter];      //  return last data point stored
                sampleCountCpltCallback(_fData);    //  call function and pass the value as parameter 
                clearDataAvailable();               //  data was read but counters don't yet match
            }
            else{
                incrementIterationCounter();
            }
        }
        else if(_fTyp == AVERAGE && isDataAvailable()){
            if(_iterationCounter >= _numSamples){
                _iterationCounter = 0;              //  reset iteration counter
                _fData = calcAvg();                 //  store data to variable;
                sampleCountCpltCallback(_fData);    //  call function and pass the value as parameter 
            }
            else{
                incrementIterationCounter();
            }
        }
        else if(_fTyp == RUN_AVG && isDataAvailable()){
            if(_iterationCounter >= _numSamples){
                _iterationCounter = 0;              //  reset iteration counter
                _fData = calcRunAvg();              //  store data to variable;
                sampleCountCpltCallback(_fData);    //  call function and pass the value as parameter 
            }
            else{
                incrementIterationCounter();
            }
        }
    }
}

int32_t A_FILTER::getData(){
    return _fData;
}

int32_t A_FILTER::getCurrentDataPoint(){
    return _data[_writeCounter];
}

void A_FILTER::clearData(){
    for(uint8_t i = 0; i < MAX_SAMPLE_CNT; i++){
        _data[i] = 0.0; //  sets all data points to 0
    }

    _fData = 0; //  set last data point to 0 as well

    #ifdef ENABLE_DEBUG
        Serial.print("Data buffer cleared.\n");
    #endif
}

void A_FILTER::sampleAndStore(){
    _data[_writeCounter] = analogRead(pin);       //  store in buffer and increment counter
    incrementWriteCounter();
    incrementIterationCounter();
}

int32_t A_FILTER::calcAvg(){
    int32_t avg = 0;    //  temp variable to store the average in
    int16_t itCnt = 0;  //  protection against while loop issue

    while(isDataAvailable()){
        avg += (_data[_readCounter])/_numSamples;   //  addition of data points divided by the number of samples takes reasonable care of any possible overflow issues
        incrementReadCounter();
        if(++itCnt >= MAX_SAMPLE_CNT){  //  we went to far, report and break
            #ifdef ENABLE_DEBUG
                Serial.print("Too many loop iterations in function loop(), while averaging data in condition else if(fTyp == AVERAGE), in line: ");
                Serial.print(__LINE__);
                Serial.print("\n");
            #endif
            break;  //  break out of while loop
        }
    }
    return avg;
}

int32_t A_FILTER::calcRunAvg(){
    int32_t avg = _fData;       //  temp variable to store the previous average in
    int16_t itCnt = 0;          //  protection against while loop overrun

    while(isDataAvailable()){
        avg += _data[_readCounter]; //  average retains value between calls - add the new value to previus average
        avg /= 2;                   //  divide by the number of data points averaged (so far, hard coded to 2)
        incrementReadCounter();

        if(++itCnt >= MAX_SAMPLE_CNT){  //  we went to far, report and break
            #ifdef ENABLE_DEBUG
                Serial.print("Too many loop iterations in function loop(), while averaging data in condition else if(fTyp == RUN_AVG), in line: ");
                Serial.print(__LINE__);
                Serial.print("\n");
            #endif
            break;  //  break out of while loop
        }
    }
    return avg;
}

bool A_FILTER::isDataAvailable(){
    return ((_writeCounter - _readCounter) > 0);    //  checks for a difference between the read and write counters
}

void A_FILTER::clearDataAvailable(){
    _readCounter = _writeCounter;   //  set the read counter to position of write counter - consider data was read
}

uint8_t A_FILTER::incrementWriteCounter(){
    _writeCounter++;
    if(_writeCounter > (MAX_SAMPLE_CNT-1))  //  wrap around handling
        _writeCounter = 0;
    if((_writeCounter - _readCounter) < 0){ //  error reporting if the read counter goes beyond the write counter
        #ifdef ENABLE_DEBUG
            Serial.print("Write counter has exceeded write counter! In line: ");
            Serial.print(__LINE__);
            Serial.print("\n");
        #endif
    }
    return _writeCounter;
}

uint8_t A_FILTER::incrementReadCounter(){
    _readCounter++;
    if(_readCounter > (MAX_SAMPLE_CNT-1))   //  wrap around handling
            _readCounter = 0;
    if((_writeCounter - _readCounter) < 0){ //  error reporting if the read counter goes beyond the write counter
        #ifdef ENABLE_DEBUG
            Serial.print("Read counter has exceeded write counter! In line: ");
            Serial.print(__LINE__);
            Serial.print("\n");
        #endif
    }
    return _readCounter;
}

uint8_t A_FILTER::incrementIterationCounter(){
    return ++_iterationCounter;                    //  simple increment
}