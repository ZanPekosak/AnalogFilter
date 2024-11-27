/*
Written by: Žan Pekošak, on: 27. 11. 2024

This is a very crude demonstration on using my library to get analog readings from Arduino pins. The idea behind this library
is that we can let all the processing happen in the background without needing to see the mess in our loop, kind of like industrial
PLC's handle their analog filtering.

You can test out the library by connecting a potentiometer between GND, A0 and 5V pins of the Arduino in the exact order.
Each filter type behaves differently. Modification of MAX and MIN paramaters is possible in the AnalogFilterConfig.h file, where
the limits are set in a conservative manner by default.
*/

#include <Arduino.h>
#include <AnalogFilter.h>

#define ANALOG_PIN A0
#define TIME_BETWEEN_SAMPLES 500
#define NUMBER_OF_SAMPLES 4
#define FILTER_TYPE AVERAGE

A_FILTER filter(ANALOG_PIN, TIME_BETWEEN_SAMPLES, NUMBER_OF_SAMPLES, FILTER_TYPE); //  set up the class constructor

void processNewDataPoint(int32_t dataPoint); //  function to call when we detect new data point arrival

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);
  filter.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  filter.loop();  //  you need this function inside loop, as it handles all the backgorund processing - for now

  /*Accessing data via polling for data point change - simple to get object specific values*/
  static uint32_t trackTime = 0;
  static int32_t prevVal = 0;
  /*You can wrap all this in another function to clean up the code. We are leaving it here, so it is more
  obvious on what happens.*/
  if((millis() - trackTime) >= 100){
    trackTime = millis();             //  set track timer
    if(prevVal != filter.dataPoint){  //  if data point change is detected
      prevVal = filter.dataPoint;     //  store last data point
      Serial.print("Change of value has been detected. Calling processing function!\n");
      processNewDataPoint(filter.dataPoint);
    }
  }

  delay(10);  //  a little delay for the arduino to rest
}

/*Accessing data within the callback function which is shared among class instances
  This function is member of the class and gets called whenever a filter function has completed calculations, no
  matter if the end result is different from the previous.
  Pin number is passed, so we can find out which of the potential multiple class instances it was called from.*/
void A_FILTER::sampleCountCpltCallback(uint8_t pin, int32_t filteredData){
  Serial.print("Filtered value: ");
  Serial.print(filteredData);
  Serial.print("\n");
}

/*This function is just a demonstration on how you might implement your own callback
  Doing it this way is often better than using the class given method, at least in the first version of the library*/
void processNewDataPoint(int32_t dataPoint){
  Serial.print("Latest datapoint value: ");
  Serial.print(dataPoint);
  Serial.print("\n");
}