#include "Arduino.h"
#include "AnalogFilter.h"
#include "AnalogFilterConfig.h"

A_FILTER::constructorError _err = BLANK;    //  set up an enum to track if there was a constructor error

A_FILTER::A_FILTER(uint8_t pin, uint16t sampleTime, uint8_t numSamples){
    _pin = pin;
    _sampleTime = sampleTime;
    if(_sampleTime < MIN_SAMPLE_TIME){
        _sampleTime = MIN_SAMPLE_TIME;  //  set the min count to the defined value in config
        _err = MINSAMPTIME;             //  set error
    }
    else if(_sampleTime > MAX_SAMPLE_TIME){
        _sampleTime = MAX_SAMPLE_TIME;  //  set the max count to the defined value in config
        _err = MAXSAMPTIME;             //  set error
    }

    _numSamples = numSamples;
    if(_numSamples < MIN_SAMPLE_CNT){
        _numSamples = MIN_SAMPLE_CNT;   //  set the min count to the defined value in config
        _err = MINSAMPLE;               //  set error
    }
    else if(_numSamples > MAX_SAMPLE_CNT){
        _numSamples = MAX_SAMPLE_CNT;   //  set the max count to the defined value in config
        _err = MAXSAMPLE;               //  set error
    }
}

void A_FILTER::begin(){
    pinMode(_pin, INPUT);

    /*Print out the warnings if any were found*/
    if(_err != BLANK){
        #ifdef ENABLE_DEBUG
            Serial.print("Warning in parameter input in class constructor A_FILTER(). ");
            Serial.print((err == MINSAMPLE) ? "Sample number less than configured minimum!" :
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
    /*If enough time has elapsed*/
    if((millis() - _timeTrack) >= _sampleTime){
        _timeTrack = millis();  //  reset the counter
        sampleAndStore();       //  store data in buffer

        /*Only call the funtion if there is any data - it should be*/
        if(isDataAvailable()){
            /*TUKAJ SI OSTAL!!!*/
            /*TUKAJ SI OSTAL!!!*/
            /*TUKAJ SI OSTAL!!!*/
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
    #ifdef ENABLE_DEBUG
        Serial.print("Data buffer cleared.\n");
    #endif
}


void A_FILTER::sampleAndStore(){
    _data[_writeCounter] = analogRead(pin);       //  store in buffer and increment counter
    incrementWriteCounter();
    incrementIterationCounter();
}

bool A_FILTER::isDataAvailable(){
    return ((_writeCounter - _readCounter) > 0);    //  checks for a difference between the read and write counters
}


void A_FILTER::incrementWriteCounter(){
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
}

void A_FILTER::incrementReadCounter(){
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
}

void A_FILTER::incrementIterationCounter(){
    _iterationCounter++;                    //  simple increment
}