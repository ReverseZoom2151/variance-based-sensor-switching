#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN 9
#define R_DIR_PIN 15
#define EMIT_PIN 11
#define FWD LOW
#define REV HIGH

int ls_pins[5] = {A11, A0, A2, A3, A4}; // stores pin numbers for convenient access

void setup() {
pinMode(10, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(15, OUTPUT);

  pinMode(A2, INPUT_PULLUP);   //line sensor 3
  pinMode(A0, INPUT_PULLUP);   //line sensor 2
  pinMode(A3, INPUT_PULLUP);   //line sensor 4
  pinMode(A11, INPUT_PULLUP);  // line sensor 1 (LEFTMOST)
  pinMode(A4, INPUT_PULLUP);  // line sensor 5 (RIGHTMOst)
  

  pinMode(EMIT_PIN, OUTPUT);
  digitalWrite(EMIT_PIN, HIGH);


 //initialising motor
  digitalWrite(10, 0);
  digitalWrite(9, 0);
  //initialising direction
  digitalWrite(16, LOW);
  digitalWrite(15, LOW);

   // Start serial, send debug text.
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");
}



int dn3;
int dn2;
int dn4;
int dn1;
int dn5;



void readAnalogSensorsWithDelay() {
    // Turn on the IR LEDs to ensure consistent lighting conditions
    pinMode(EMIT_PIN, OUTPUT);
    digitalWrite(EMIT_PIN, HIGH);

    // Introduce a delay to allow observations between sets of readings
    const int readingDelay = 3000; // Delay in milliseconds, adjust as needed

    while (true) { // Infinite loop to continuously read sensors; break it as needed
        Serial.println("Sensor Readings:");
        for (int i = 0; i < 5; i++) {
            int sensorValue = analogRead(ls_pins[i]); // Use analogRead for analog sensors
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(sensorValue);
        }

        // Wait for a specified time before reading the sensors again
        Serial.println("Wait for next set of readings...");
        delay(readingDelay);

        // To prevent the LEDs from being on all the time and possibly affecting the sensors' response over time,
        // you can turn off the IR LEDs during the delay and then turn them back on before the next readings.
        digitalWrite(EMIT_PIN, LOW); // Turn off IR LEDs during the delay to mimic operational breaks
        delay(100); // Short break to ensure LEDs are off for a moment
        digitalWrite(EMIT_PIN, HIGH); // Turn back on before the next set of readings
    }
}

unsigned long  readSensorTimeElapsed(int sensorPin) {
  // Set the sensor pin as an OUTPUT and HIGH to "charge" it
  pinMode(sensorPin, OUTPUT);
  digitalWrite(sensorPin, HIGH);

  // Small delay to allow charging 
  delayMicroseconds(10);  // Adjust this tiny delay if needed

  // Switch the pin to INPUT mode
  pinMode(sensorPin, INPUT);

  // Start timing
  unsigned long startTime = micros();

  // Wait for the pin to go LOW (discharge)
  while (digitalRead(sensorPin) == HIGH) {}

  // End timing and calculate elapsed time
  unsigned long endTime = micros();
  return endTime - startTime; 
}




void readDigitalSensorsWithDelay() {
    // Turn on the IR LEDs to ensure consistent lighting conditions
    pinMode(EMIT_PIN, OUTPUT);
    digitalWrite(EMIT_PIN, HIGH);

    // Introduce a delay to allow observations between sets of readings
    const int readingDelay = 3000; // Delay in milliseconds, adjust as needed

    while (true) { 
    Serial.println("Sensor Readings (Time Elapsed):");

    // Use readSensorTimeElapsed function for each sensor:
    for (int i = 0; i < 5; i++) {
      unsigned long timeElapsed = readSensorTimeElapsed(ls_pins[i]);
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(timeElapsed);
    }

        // Wait for a specified time before reading the sensors again
        Serial.println("Wait for next set of readings...");
        delay(readingDelay);

        digitalWrite(EMIT_PIN, LOW); // Turn off IR LEDs during the delay to mimic operational breaks
        delay(100); 
        digitalWrite(EMIT_PIN, HIGH); // Turn back on before the next set of readings
    }
}

void loop() {

// Read sensors using the timed method
  unsigned long timeElapsed1 = readSensorTimeElapsed(A11); // Leftmost
  unsigned long timeElapsed2 = readSensorTimeElapsed(A0);
  unsigned long timeElapsed3 = readSensorTimeElapsed(A2); // Center
  unsigned long timeElapsed4 = readSensorTimeElapsed(A3);
  unsigned long timeElapsed5 = readSensorTimeElapsed(A4); // Rightmost


  dn3 = analogRead(A2);
  dn2 = analogRead(A0);
  dn4 = analogRead(A3);
  dn1 = analogRead(A11);
  dn5 = analogRead(A4);



  // Serial.print(dn1);
  // Serial.print(",");
  // Serial.print(dn2);
  // Serial.print(",");
  // Serial.print(dn3);
  // Serial.print(",");
  // Serial.print(dn4);
  // Serial.print(",");
  // Serial.print(dn5);
  // Serial.print("\n");
}
