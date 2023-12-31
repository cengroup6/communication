
#include "DFRobot_Heartrate.h"

//rotatory
#define resetPin 2
#define inputDT 4
#define inputCLK 6

float rotationStep = 0.1; // Change this value to suit your needs
float rotationValue = 0; // Cumulative rotation value


int counter = 0; 
int currentStateCLK;
int previousStateCLK; 
unsigned long lastRotaryInterruptTime = 0;
const unsigned long debounceDelay = 0; // Adjust this value for better debouncing

//reed sensor
#define reed  12 // The Hall sensor's output is connected to analog pin A0
unsigned long previousMillis = 0; // Stores the last time a command was sent
const long interval = 500;       // Interval at which to send command (2000 milliseconds = 2 seconds)


unsigned long lastPulseTime = 0;
unsigned long pulseTime;
float speedMph;

//heart rate
#define heartratePin A1
DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE



void setup() { 

  

  //reed-sensor
  pinMode(reed,INPUT);
  //rotatory
  pinMode(resetPin, INPUT_PULLUP);
  pinMode(inputCLK, INPUT);
  pinMode(inputDT, INPUT);
  
  Serial.begin(9600);
  previousStateCLK = digitalRead(inputCLK);//rotatory
} 



void loop(){
  reedSensor();
  rotatory();
  heartRate();
}


void heartRate() {
  uint8_t rateValue;
  heartrate.getValue(heartratePin); ///< A1 foot sampled values
  rateValue = heartrate.getRate(); ///< Get heart rate value 
  if(rateValue)  {
  	Serial.print("b: ");
    	Serial.println(rateValue);
  }
}



void reedSensor(){
  unsigned long currentMillis = millis();

  if(digitalRead(reed) == 1){
    // When the reed sensor is activated
    Serial.print("s: ");
    Serial.println(.75);  // Send the speed value for forward motion
    previousMillis = currentMillis;  // Update the last command time
  } else {
    // When the reed sensor is not activated
    if (currentMillis - previousMillis >= interval) {
      // If 2 seconds have passed
      Serial.print("s: ");
      Serial.println(0);   // Send 0 to indicate stopping or slowing down
      previousMillis = currentMillis; // Update the last command time
    }
  }
}


void rotatory() {
    currentStateCLK = digitalRead(inputCLK);

    if (currentStateCLK != previousStateCLK) {
        lastRotaryInterruptTime = millis(); // Update the last interrupt time

        if (digitalRead(inputDT) != currentStateCLK) {
            rotationValue += rotationStep; // Turned left
        } else {
            rotationValue -= rotationStep; // Turned right
        }

        // Send the cumulative rotation value
        Serial.print("r: ");
        Serial.println(rotationValue);
    }
    previousStateCLK = currentStateCLK;
    if (digitalRead(resetPin) == LOW) {
        // Button is pressed
        Serial.print("y: ");
        Serial.println(1);
    }
}
