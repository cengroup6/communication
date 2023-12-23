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

//hall-effect
const int hallPin = A0; // The Hall sensor's output is connected to analog pin A0
const int ledPin = 13;  // Use the built-in LED to indicate a magnetic field detection
const int threshold = 520; // Set the threshold for detection to mid-range of analog input
const float wheelCircumference = 2.0; // Wheel circumference in meters

unsigned long lastPulseTime = 0;
unsigned long pulseTime;
float speedMph;




void setup() { 

  //hall-effect
  pinMode(ledPin, OUTPUT);
  //rotatory
  pinMode(inputCLK, INPUT);
  pinMode(inputDT, INPUT);
  pinMode(ledCW, OUTPUT);
  pinMode(ledCCW, OUTPUT);
  
  Serial.begin(9600);
  previousStateCLK = digitalRead(inputCLK);//rotatory
} 



void loop(){
  hallEffect();
  rotatory();

}


void hallEffect() {
  int hallValue = analogRead(hallPin); // Read the Hall sensor
//  Serial.print("Analog value:");
//  Serial.println(hallValue);
  
  if (hallValue > threshold) {
    digitalWrite(ledPin, HIGH); // If the sensor's output is high, turn on the LED
    unsigned long currentTime = millis();
    if (currentTime - lastPulseTime > 50) { // Debounce for 50 ms
      pulseTime = currentTime - lastPulseTime;
      lastPulseTime = currentTime;
      speedMph = calculateSpeed(pulseTime);
      Serial.print("Speed: ");
      Serial.println(speedMph);
    }
    //Serial.println("1");
  } else {
    digitalWrite(ledPin, LOW); // If the sensor's output is low, turn off the LED
    //Serial.println("0");
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
      //Serial.println("Direction: CCW -- Value: " + String(counter));
    } else {
      counter++;
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, LOW);
      //Serial.println("Direction: CW -- Value: " + String(counter));
    }
  } 
  previousStateCLK = currentStateCLK;
}
