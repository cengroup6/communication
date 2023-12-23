//heart rate
const int SENSOR_PIN = 0; //define sensor pin
unsigned long previousMillis = 0; // will store last time a beat was detected
const long interval = 5000; // interval at which to count beats (1 second)
int beatCount = 0; // number of beats detected
int lastBeatValue = 0; // the last read value
void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
}

void loop() {
 int sensorValue = analogRead(SENSOR_PIN); // read the value from the sensor

    // Check for a beat
    if (sensorValue < 950 && lastBeatValue >= 1023) {
        // A beat is detected
        beatCount++;
    }
    lastBeatValue = sensorValue;

    // Get the current time
    unsigned long currentMillis = millis();

    // Check if ten seconds have passed; update BPM
    if (currentMillis - previousMillis >= interval) {
        // Save the last time you updated the BPM
        previousMillis = currentMillis;

        // Calculate BPM
        int bpm = beatCount * 12; // Multiply by 6 to convert to beats per minute

        // Print BPM
        Serial.print("BPM: ");
        Serial.println(bpm);

        // Reset beat count
        beatCount = 0;
    }

    // Short delay to stabilize
  delay(10);
}
