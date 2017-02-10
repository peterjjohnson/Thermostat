#include <SparkFunHTU21D.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>

const int RELAY = 7; // Use pin 7 to control the relay
const char ON = LOW; // The relay closes when it receives a LOW signal
const char OFF = HIGH; // The relay opens when it receives a HIGH signal

// Set up our globals
float humidity = 0;
float realTemp = 0;
float holdTemp = 18;
float threshold = 1;
char furnaceState = OFF;

// Create instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barometric sensor
Weather sensor;

// Get the hardware ready
void setup()
{
  Serial.begin(9600);   // open serial over USB at 9600 baud
  pinMode(RELAY, OUTPUT);   // initialize pin 7 for output
  sensor.begin();       // connect to our temp and humidity sensor
}

// The main event
void loop()
{
  updateStats(); // Update our temp and humidity stats
  setFurnaceState(); // Turn the furnace on or off if we need to
  
  // If there's data in the serial buffer try and use it to adjust the temp
  if (Serial.available()) {
    processRequest(Serial.readString());
  }
    
  printInfo(); // Print current info
  
  delay(100);
}

// Get current temp and humidity stats
void updateStats()
{
  // Measure current relative humidity
  humidity = sensor.getRH();
  // Measure current temperature
  realTemp = sensor.getTemp();
}

// Parse and process requests via serial port
void processRequest(String request)
{
  String command = request.substring(request.lastIndexOf(";") + 1, request.lastIndexOf(":"));
  String value = request.substring(request.lastIndexOf(":") + 1);
  
  if (command == "setTemp") {
    holdTemp = value.toFloat();
  } else {
    Serial.print("Unrecognized request\n");
  }
}

// Turn the furnace on or off as necessary
void setFurnaceState() 
{
  int tempdiff = holdTemp - realTemp;
  char newState = furnaceState;
  
  if (tempdiff >= threshold) {
    // Current temp is more than threshold below holdtemp, turn the furnace on
    newState = ON;
  } else if (abs(tempdiff) >= threshold) {
    // Current temp is more than threshold above holdtemp, turn the furnace off
    newState = OFF;
  }
  
  if (newState != furnaceState) {
    furnaceState = newState;
    digitalWrite(RELAY, furnaceState);
  }
}

// Print current stats and settings to the serial output in JSON format
void printInfo()
{

  Serial.print("{");
  
  Serial.print("\"Temp\":");
  Serial.print(realTemp);
  Serial.print(",");
  
  Serial.print("\"Humidity\":");
  Serial.print(humidity);
  Serial.print(",");
  
  Serial.print("\"HoldTemp\":");
  Serial.print(holdTemp);
  Serial.print(",");
  
  Serial.print("\"Furnace\":");

  if (furnaceState == ON) {
    Serial.print("\"ON\"");
  } else {
    Serial.print("\"OFF\"");
  }

  Serial.println("}");
}

