#ifndef DIGITAL_H
#define DIGITAL_H
#define MAX_SAMPLES 10
#define EMIT_PIN 11
#define LS_LEFT_PIN A11  
#define LS_MIDLEFT_PIN A0   
#define LS_MIDDLE_PIN A2   
#define LS_MIDRIGHT_PIN A3  
#define LS_RIGHT_PIN A4  

class Digital_c {

  private:

      int ls_pins[5] = {A11, A0, A2, A3, A4}; // stores pin numbers for convenient access
      unsigned long readings[5][MAX_SAMPLES]; // stores readings for each sensor

  public:

    Digital_c() {}

    unsigned long readSensorTimeElapsed(int sensorPin) {

      // Existing readSensorTimeElapsed function
      pinMode(sensorPin, OUTPUT);
      digitalWrite(sensorPin, HIGH);
      delayMicroseconds(10);
      pinMode(sensorPin, INPUT);

      unsigned long startTime = micros();

      while (digitalRead(sensorPin) == HIGH) {}

      unsigned long endTime = micros();

      return endTime - startTime; 

    }

    void collectReadings() {

      pinMode(EMIT_PIN, OUTPUT);
      digitalWrite(EMIT_PIN, HIGH);

      for (int sensor = 0; sensor < 5; sensor++) {

        for (int sample = 0; sample < MAX_SAMPLES; sample++) {

          readings[sensor][sample] = readSensorTimeElapsed(ls_pins[sensor]);

        }
          
      }

      digitalWrite(EMIT_PIN, LOW);

    }

    float calculateVariance(int sensorIndex) {

      unsigned long sum = 0;
      float mean;
      float variance = 0.0;

      // Find the minimum and maximum values for normalization
      unsigned long minVal = readings[sensorIndex][0];
      unsigned long maxVal = readings[sensorIndex][0];

      for (int i = 0; i < MAX_SAMPLES; i++) {

        minVal = min(minVal, readings[sensorIndex][i]);
        maxVal = max(maxVal, readings[sensorIndex][i]);

      }

      // Normalize the readings
      float normalizedReadings[MAX_SAMPLES];

      for (int i = 0; i < MAX_SAMPLES; i++) {

        normalizedReadings[i] = (float)(readings[sensorIndex][i] - minVal) / (maxVal - minVal);

      }

      // Calculate mean of normalized readings
      for (int i = 0; i < MAX_SAMPLES; i++) {

        sum += normalizedReadings[i];

      }

      mean = sum / MAX_SAMPLES;

      // Calculate variance of normalized readings
      for (int i = 0; i < MAX_SAMPLES; i++) {

        variance += pow((normalizedReadings[i] - mean), 2);

      }

      variance /= MAX_SAMPLES;

      return variance;
      
    }

    void readDigitalSensorsWithVariance() {

      collectReadings();

      Serial.println("Sensor Readings (Time Elapsed) and Variances:");

      for (int sensor = 0; sensor < 5; sensor++) {

        Serial.print("Sensor ");
        Serial.print(sensor);
        Serial.print(": ");

        for (int sample = 0; sample < MAX_SAMPLES; sample++) {

          Serial.print(readings[sensor][sample]);
          Serial.print(" ");

        }

        float variance = calculateVariance(sensor);

        Serial.print("Variance: ");
        Serial.println(variance);

      }

    }

};

#endif

