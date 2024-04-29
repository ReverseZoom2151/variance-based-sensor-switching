#ifndef ANALOGLINESENSORS_H
#define ANALOGLINESENSORS_H
#define MAX_SAMPLES 10
#define EMIT_PIN    11
#define LS_LEFT_PIN A11
#define LS_MIDLEFT_PIN A0
#define LS_MIDDLE_PIN A2
#define LS_MIDRIGHT_PIN A3
#define LS_RIGHT_PIN A4

class AnalogLineSensors_c {

  private:

    int ls_pins[5] = {LS_LEFT_PIN, LS_MIDLEFT_PIN, LS_MIDDLE_PIN, LS_MIDRIGHT_PIN, LS_RIGHT_PIN}; // stores pin numbers for convenient access
    int sensorReadings[5]; // array to store readings

    // Paul: variables to store calibration values
    //       for each sensor, which are then applied
    //       to all future sensor readings.
    float offset[5];
    float scale[5];

  public:

    float calibrated[5]; // to store normalised readings later.
    float variance[5];

    AnalogLineSensors_c() {}

    void setupAllLineSensors() {

      pinMode(EMIT_PIN, OUTPUT);
      digitalWrite(EMIT_PIN, HIGH); // turn on IR LEDs

      for (int i = 0; i < 5; i++) {

        // Paul: this needs to be INPUT_PULLUP
        //pinMode(ls_pins[i], INPUT);
        pinMode(ls_pins[i], INPUT_PULLUP);
      
      }

    }

    unsigned long readLineSensor(int number) {

      // Check for valid sensor number to prevent out-of-bounds access
      if (number < 0 || number >= 5) {

        Serial.println("Error: sensor number out of range");
        
        return -1; // Return an error indication
      
      }

      // Turn on IR LED to ensure reflective readings are consistent
      pinMode(EMIT_PIN, OUTPUT);
      digitalWrite(EMIT_PIN, HIGH);

      // No need to set pinMode for the sensor pin as INPUT_PULLUP,
      // because analogRead automatically configures the pin as an input.
      // Delay is not strictly necessary for analogRead but could be included
      // for consistency with digital version or specific sensor characteristics.
      delayMicroseconds(10);

      // Read the analog value from the specified sensor
      int reading = analogRead(ls_pins[number]);

      // Optionally, turn off the IR LED if you want to conserve power
      // and if it doesn't interfere with subsequent readings
      digitalWrite(EMIT_PIN, LOW);

      // Return the reading. Note that we're keeping the return type as unsigned long
      // for consistency with the digital version, even though analog readings are int.
      // This is a design choice; adjust as necessary for your application.
      return (unsigned long)reading;

    }

    void readAllSensors() {

      // Paul: adding the below to ensure the line
      //       sensors are set up properly before
      //       an analog read.
      setupAllLineSensors();

      // collects MAX_SAMPLES readings for each of the 5 sensors
      for (int i = 0; i < MAX_SAMPLES; i++) {

        for (int sensor_num = 0; sensor_num < 5; sensor_num++) {

          sensorReadings[sensor_num] = (int)analogRead(ls_pins[sensor_num]);

        }

      }

    }

    void calibrate() {
      
      float min_values[5];
      float max_values[5];

      // Initialise
      for (int i = 0; i < 5; i++) {

        min_values[i] = 9999.9;
        max_values[i] = -9999.9;

      }

      int count = 0;
      
      while (count < 50 ) { // repeat 50 times

        readAllSensors();
        
        for (int i = 0; i < 5; i++ ) {

          if (sensorReadings[i] > max_values[i]) {
            
            max_values[i] = sensorReadings[i];

          }

          if (sensorReadings[i] < min_values[i]) {
            
            min_values[i] = sensorReadings[i];

          }

        }

        // 10ms * 50 = 0.5 seconds to capture min/max
        delay(10);

        count++;
        
      }

      // Determine calibration values
      for (int i = 0; i < 5; i++) {
        
        offset[i] = min_values[i];

        // Being cautious with type conversion (int to float)
        float temp;
        temp = max_values[i];  // Get max
        temp -= min_values[i]; // subtract min, gives range.
        temp = 1.0 / temp;     // 1 over creates a scaling factor
        scale[i] = temp;
      
      }

      // Done
    
    }

    // You can call this to get calibrated sensor readings
    // during the operation of the robot.
    void getCalibrated() {
      
      readAllSensors();

      for (int i = 0; i < 5; i++) {

        calibrated[i] = (float)sensorReadings[i];
        calibrated[i] -= offset[i];
        calibrated[i] *= scale[i];

      }

    }

    // Paul: helpful
    void printCalibrated() {
      
      for (int i = 0; i < 5; i++) {

        Serial.print(calibrated[i]);
        Serial.print(",");

      }
      
      Serial.print("\n");
      
    }

    // Calculate the variance of a set of normalized readings
    // Paul: I think we can save a bit of memory by collecting
    //       our samples within this function.
    float calculateVariance() {

      unsigned long start_time = millis();

      // 5 sensors, 10 samples each
      int num_samples = 10;

      float samples[5][num_samples];
      float sum[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
      float mean[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

      // Collect samples
      for (int sample = 0; sample < num_samples; sample++) {

        getCalibrated();

        // Copy for each sensor into samples array
        for (int sensor = 0; sensor < 5; sensor++) {
        
          samples[sensor][sample] = calibrated[sensor];

          // Might as well capture sum whilst were here
          sum[sensor] += calibrated[sensor];
        
        }

      }

      // Calculate means
      for (int sensor = 0; sensor < 5; sensor++) {
        
        mean[sensor] = sum[sensor] / (float)num_samples;
      
      }
      
 
      for (int sensor = 0; sensor < 5; sensor++) {
          
        variance[sensor] = 0.0;

        for (int sample = 0; sample < num_samples; sample++) {

          variance[sensor] += pow(samples[sensor][sample] - mean[sensor], 2);
        
        }
      
        variance[sensor] /= (float)num_samples;

      }

      unsigned long end_time = millis();

      // Paul: I found this takes around 60ms and is
      //       consistent, which is surprising
      // Serial.print("Analogue Variance calculation took: ");
      // Serial.println( (int)( end_time - start_time ) );
    
    }
    
    // Paul: helpful
    void printVariance() {
      
      for (int i = 0; i < 5; i++) {

        Serial.print(variance[i], 6); // (, 6) = print 6 decimal places
        Serial.print(",");

      }
    
      Serial.print("\n");
    
    }

    // Paul: helpful
    void printReadings() {
      
      for (int i = 0; i < 5; i++) {

        Serial.print(sensorReadings[i]);
        Serial.print(",");
      
      }
      
      Serial.print("\n");
    
    }

    float calculateAverageVariance() {

      float totalVariance = 0.0;
      
      calculateVariance();
      
      for (int i = 1; i < 4; i++) {
      
        totalVariance += variance[i];
        Serial.println(totalVariance, 6);
        Serial.print(",");

        delay(10);
      
      }

      return totalVariance / 5.0f; 
      
  
    }

};

#endif
