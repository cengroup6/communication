//Parameters
const int clkPin   = 2;
const int dtPin   = 3;
const int swPin   = 4;

//Variables
int rotVal  = 0;
bool clkState   = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast   = HIGH;

void setup() {
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Init Rotary
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);
}

void loop() {
  readRotary();
}

void readRotary( ) { /* function readRotary */
  ////Test routine for Rotary
  // gestion position
  clkState = digitalRead(clkPin);
  if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
      Serial.print("Rotary position ");
      if (digitalRead(dtPin) == HIGH) {
          rotVal = rotVal - 1;
          if ( rotVal < -10 ) {
              rotVal = 0;
          }
      }
      else {
          rotVal++;
          if ( rotVal > 10 ) {
              rotVal = 0;
          }
      }
      Serial.println(rotVal);
      delay(200);
  }
  clkLast = clkState;

  //gestion bouton
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH) {
      Serial.println("Rotary pressed");
      delay(100);//debounce
  }
  swLast = swState;
}


//modify this for 12th
// void rotatory() {
//     currentStateCLK = digitalRead(inputCLK);

//     if (currentStateCLK != previousStateCLK && (millis() - lastRotaryInterruptTime) > debounceDelay) {
//         lastRotaryInterruptTime = millis(); // Update the last interrupt time

//         if (digitalRead(inputDT) != currentStateCLK) {
//             counter--; // Turned left
//         } else {
//             counter++; // Turned right
//         }

//         // Determine the state based on the counter
//         if (counter > 0) {
//             digitalWrite(ledCW, HIGH);
//             digitalWrite(ledCCW, LOW);
//             Serial.println("r: 1");
//         } else if (counter < 0) {
//             digitalWrite(ledCW, LOW);
//             digitalWrite(ledCCW, HIGH);
//             Serial.println("r:-1");
//         } else { // counter == 0
//             digitalWrite(ledCW, LOW);
//             digitalWrite(ledCCW, LOW);
//             Serial.println("r: 0");
//         }
//     }

//     previousStateCLK = currentStateCLK;
// }
