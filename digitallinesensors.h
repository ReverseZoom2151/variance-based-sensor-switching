#ifndef DIGITALLINESENSORS_H
#define DIGITALLINESENSORS_H
#define MAX_SAMPLES 10
#define EMIT_PIN    11  
#define LS_LEFT_PIN 12  
#define LS_MIDLEFT_PIN A0   
#define LS_MIDDLE_PIN A2   
#define LS_MIDRIGHT_PIN A3  
#define LS_RIGHT_PIN A4   
#define LINE_SENSOR_UPDATE 100
#define MOTOR_UPDATE 2000 

class DigitalLineSensors_c {

  private: 

    int ls_pins[5] = { LS_LEFT_PIN, LS_MIDLEFT_PIN, LS_MIDDLE_PIN, LS_MIDRIGHT_PIN, LS_RIGHT_PIN }; // stores pin numbers for convenient access
    float results[MAX_SAMPLES];
    unsigned long sensorReadings[5]; // array to store readings

  public:

    DigitalLineSensors_c() {}

    void setupAllLineSensors() {

      pinMode(EMIT_PIN, INPUT);  
      for (int i = 0; i < 5; i++) {
        pinMode(ls_pins[i], INPUT);
      }

    }

    void algorithm_2() {

      // collects 10 sensor readings and store in results[]
      for (int i = 0; i < MAX_SAMPLES; i++) {

        // turns on IR LEDs directly
        pinMode(EMIT_PIN, OUTPUT); 
        digitalWrite(EMIT_PIN, HIGH); 

        pinMode(LS_LEFT_PIN, OUTPUT);
        digitalWrite(LS_LEFT_PIN, HIGH);
        delayMicroseconds(10);  

        pinMode(LS_LEFT_PIN, INPUT); 

        unsigned long start_time = micros();
        unsigned long timeout = 5000; 

        while (digitalRead(LS_LEFT_PIN) == HIGH) {

          unsigned long current_time = micros();

          if ((current_time - start_time) > timeout) {
            results[i] = -1; 
            break; 
          }

        }

        // obtains end_time only if successful reading 
        unsigned long end_time = (results[i] != -1) ? micros() : 0;  

        pinMode(EMIT_PIN, INPUT); // turns off EMIT 

        if (results[i] != -1) {  

          unsigned long elapsed_time = end_time - start_time;
          results[i] = elapsed_time;
        
        }

        delay(200); // optional delay between measurements

      }

      // reports all readings / results, forever 
      while (true) {

        Serial.println("Results: ");

        for (int i = 0; i < MAX_SAMPLES; i++) {
          Serial.println(results[i]);
        }

        // waits before next trial (optional)
        delay(1000);

      }

    }

    // reads a line sensor with error checking
    unsigned long readLineSensor(int number) {

      // prevents memory errors 
      if (number < 0 || number > 4) {

        Serial.println("Error: sensor number out of range");
        return -1; // or some other clear error indication
     
      }

      pinMode(EMIT_PIN, OUTPUT);
      digitalWrite(EMIT_PIN, HIGH);
      pinMode(ls_pins[number], OUTPUT);
      digitalWrite(ls_pins[number], HIGH);
      delayMicroseconds(10); 

      pinMode(ls_pins[number], INPUT); // only switches to input for measurement

      unsigned long start_time = micros();

      while (digitalRead(ls_pins[number]) == HIGH) {}

      unsigned long end_time = micros();

      pinMode(EMIT_PIN, INPUT); 

      unsigned long elapsed_time = end_time - start_time;
      
      return elapsed_time; 

    } 

    void multipleSensors() {

      // collects MAX_SAMPLES readings for each of the 5 sensors
      for (int i = 0; i < MAX_SAMPLES; i++) {

        for (int sensor_num = 0; sensor_num < 5; sensor_num++) {

          sensorReadings[sensor_num] = readLineSensor(sensor_num); 
          delay(200); 

        }

        // reports results for set of readings
        Serial.print("[");
        Serial.print(i); // greyscale index (adjust if needed)

        for (int sensor_num = 0; sensor_num < 5; sensor_num++) {

          Serial.print(", ");
          Serial.print(sensorReadings[sensor_num]);
          
        }

        Serial.println("]");

      }

      // Normalize and calculate variance for each sensor
      for (int sensor_num = 0; sensor_num < 5; sensor_num++) {
        normalizeReadings(sensorReadings, MAX_SAMPLES);
        float variance = calculateVariance(sensorReadings, MAX_SAMPLES);
        Serial.print("Variance for sensor ");
        Serial.print(sensor_num);
        Serial.print(": ");
        Serial.println(variance);
      }

    }

    // Normalize a set of readings to be between 0 and 1
    void normalizeReadings(unsigned long readings[], int numReadings) {
        unsigned long minVal = readings[0];
        unsigned long maxVal = readings[0];
        
        // Find the min and max values
        for (int i = 1; i < numReadings; i++) {
            if (readings[i] < minVal) {
                minVal = readings[i];
            }
            if (readings[i] > maxVal) {
                maxVal = readings[i];
            }
        }
        
        // Normalize the readings
        for (int i = 0; i < numReadings; i++) {
            readings[i] = (float)(readings[i] - minVal) / (maxVal - minVal);
        }
    }

    // Calculate the variance of a set of normalized readings
    float calculateVariance(unsigned long readings[], int numReadings) {
        float sum = 0.0;
        float mean = 0.0;
        
        // Calculate the mean
        for (int i = 0; i < numReadings; i++) {
            sum += readings[i];
        }
        mean = sum / numReadings;
        
        // Calculate the variance
        float variance = 0.0;
        for (int i = 0; i < numReadings; i++) {
            variance += pow(readings[i] - mean, 2);
        }
        variance /= (numReadings - 1);
        
        return variance;
    }

};

#endif
