
#include "DFRobot_Heartrate.h"

//rotatory
#define inputCLK 4
#define inputDT 5
#define ledCW 8
#define ledCCW 9

int counter = 0; 
int currentStateCLK;
int previousStateCLK; 
unsigned long lastRotaryInterruptTime = 0;
const unsigned long debounceDelay = 100; // Adjust this value for better debouncing

//reed sensor
#define reed  12 // The Hall sensor's output is connected to analog pin A0
const int ledPin = 13;  // Use the built-in LED to indicate a magnetic field detection
const int threshold = 520; // Set the threshold for detection to mid-range of analog input
const float wheelCircumference = 2.0; // Wheel circumference in meters

unsigned long lastPulseTime = 0;
unsigned long pulseTime;
float speedMph;

//heart rate
#define heartratePin A1
DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE



void setup() { 

  //hall-effect
  pinMode(ledPin, OUTPUT);
  pinMode(reed,INPUT);
  //rotatory
  pinMode(inputCLK, INPUT);
  pinMode(inputDT, INPUT);
  pinMode(ledCW, OUTPUT);
  pinMode(ledCCW, OUTPUT);
  
  Serial.begin(9600);
  previousStateCLK = digitalRead(inputCLK);//rotatory
} 



void loop(){
  //reedSensor();
  //rotatory();
  heartRate();
  delay(10);
 

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
  if(digitalRead(reed) == 1){
    digitalWrite(ledPin,HIGH);
    unsigned long currentTime = millis();
    if (currentTime - lastPulseTime > 50) { // Debounce for 50 ms
      pulseTime = currentTime - lastPulseTime;
      lastPulseTime = currentTime;
      speedMph = calculateSpeed(pulseTime);
      if(speedMph < 25.00){
        Serial.print("s: ");
        Serial.println(speedMph);
      } 
     
    }
  }else{
    digitalWrite(ledPin,LOW);  
  }
  
}



float calculateSpeed(unsigned long timeMillis) {
  float distanceMiles = wheelCircumference / 1000.0 / 1.609; // Convert meters to miles
  float timeHours = timeMillis / 1000.0 / 3600.0; // Convert milliseconds to hours
  return distanceMiles / timeHours; // Speed in mph
}

void rotatory() { 
  currentStateCLK = digitalRead(inputCLK);
  if (currentStateCLK != previousStateCLK && (millis() - lastRotaryInterruptTime) > debounceDelay) {
    lastRotaryInterruptTime = millis(); // Update the last interrupt time

    if (digitalRead(inputDT) != currentStateCLK) { 
      counter--;
      digitalWrite(ledCW, LOW);
      digitalWrite(ledCCW, HIGH);
      Serial.println("d: right");
    } else {
      counter++;
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, LOW);
      Serial.println("d: left ");
    }
  } 
  previousStateCLK = currentStateCLK;
}
