#ifndef DIGITALLINESENSORS_H
#define DIGITALLINESENSORS_H

#define MAX_SAMPLES 10

#define EMIT_PIN    11

#define LS_LEFT_PIN 12
#define LS_MIDLEFT_PIN A0
#define LS_MIDDLE_PIN A2
#define LS_MIDRIGHT_PIN A3
#define LS_RIGHT_PIN A4

class DigitalLineSensors_c {

  private:

    int ls_pins[5] = { LS_LEFT_PIN, LS_MIDLEFT_PIN, LS_MIDDLE_PIN, LS_MIDRIGHT_PIN, LS_RIGHT_PIN }; // stores pin numbers for convenient access

    int sensorReadings[5]; // array to store readings

    // to store normalised readings later.
    float calibrated[5];

    // Paul: variables to store calibration values
    //       for each sensor, which are then applied
    //       to all future sensor readings.
    float offset[5];
    float scale[5];

    // variance for each sensor
    float variance[5];

  public:

    DigitalLineSensors_c() {}

    // Paul: this isn't so useful for a digital read.
    //       commenting out.
    //    void setupAllLineSensors() {
    //
    //      pinMode(EMIT_PIN, INPUT);
    //
    //      for (int i = 0; i < 5; i++) {
    //        pinMode(ls_pins[i], INPUT);
    //      }
    //
    //    }


    void calibrate() {
      int min_values[5];
      int max_values[5];

      // Initialise
      for ( int i = 0; i < 5; i++ ) {
        min_values[i] = 9999.9;
        max_values[i] = -9999.9;
      }

      int count = 0;
      while ( count < 50 ) { // repeat 50 times

        readAllSensors();
        for ( int i = 0; i < 5; i++ ) {
          if ( sensorReadings[i] > max_values[i] ) max_values[i] = sensorReadings[i];
          if ( sensorReadings[i] < min_values[i] ) min_values[i] = sensorReadings[i];
        }

        // 10ms * 50 = 1 second to capture min/max
        delay(10);

        count++;
      }

      // Determine calibration values
      for ( int i = 0; i < 5; i++ ) {
        offset[i] = (float)min_values[i];

        // Being cautious with type conversion (int to float)
        float temp;
        temp = (float)max_values[i];  // Get max
        temp -= (float)min_values[i]; // subtract min, gives range.
        temp = 1.0 / temp;            // 1 over creates a scaling factor
        scale[i] = temp;
      }

      // done
    }

    // You can call this to get calibrated sensor readings
    // during the operation of the robot.
    void getCalibrated() {
      readAllSensors();

      for ( int i = 0; i < 5; i++ ) {
        calibrated[i] = (float)sensorReadings[i];
        calibrated[i] -= offset[i];
        calibrated[i] *= scale[i];
      }

    }

    // Paul: helpful
    void printCalibrated() {
      for ( int i = 0; i < 5; i++ ) {
        Serial.print( calibrated[i] );
        Serial.print(",");
      }
      Serial.print("\n");
    }
    // Paul: helpful
    void printVariance() {
      for ( int i = 0; i < 5; i++ ) {
        Serial.print( variance[i], 6 ); // ,6 = print 6 decimal places
        Serial.print(",");
      }
      Serial.print("\n");
    }



    // Paul: reads all line sensors in a single function
    void readAllSensors() {

      // Turn on IR LED
      pinMode( EMIT_PIN, OUTPUT );
      digitalWrite( EMIT_PIN, HIGH );

      // Charge capacitors
      for ( int i = 0; i < 5; i++ ) {
        pinMode( ls_pins[i], OUTPUT );
        digitalWrite( ls_pins[i], HIGH );

        // Let's initialise sensor readings to a
        // known value too.
        sensorReadings[i] = 0;

      }
      delayMicroseconds(10);

      // Set all sensor pins back to input
      for ( int i = 0; i < 5; i++ ) pinMode( ls_pins[i], INPUT );


      // Now to read...
      int remaining = 5;
      unsigned long start_time = micros();
      while ( remaining > 0 ) { // still have sensors to complete?

        // check each sensor
        for ( int i = 0; i < 5; i++ ) {

          // Still has initial value? hasn't completed yet.
          if ( sensorReadings[ i ] == 0 ) {

            // If low, now we record time elapsed
            if ( digitalRead( ls_pins[i] ) == LOW ) {
              unsigned long dt =  micros() - start_time;
              sensorReadings[i] = (int)dt;

              // Reduce count as this one has completed.
              remaining--;
            } // if low

          } // if initial value

        } // for each sensor

      } // while remaining

      // done.

    }

    // Paul: helpful
    void printReadings() {
      for ( int i = 0; i < 5; i++ ) {
        Serial.print( sensorReadings[i] );
        Serial.print(",");
      }
      Serial.print("\n");
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

    //    void multipleSensors() {
    //
    //      // collects MAX_SAMPLES readings for each of the 5 sensors
    //      for (int i = 0; i < MAX_SAMPLES; i++) {
    //
    //        for (int sensor_num = 0; sensor_num < 5; sensor_num++) {
    //
    //          sensorReadings[sensor_num] = readLineSensor(sensor_num);
    //          delay(200);
    //
    //        }
    //
    //        // reports results for set of readings
    //        Serial.print("[");
    //        Serial.print(i); // greyscale index (adjust if needed)
    //
    //        for (int sensor_num = 0; sensor_num < 5; sensor_num++) {
    //
    //          Serial.print(", ");
    //          Serial.print(sensorReadings[sensor_num]);
    //
    //        }
    //
    //        Serial.println("]");
    //
    //      }
    //
    //      // Normalize and calculate variance for each sensor
    //      for (int sensor_num = 0; sensor_num < 5; sensor_num++) {
    //
    //        normalizeReadings(sensorReadings, MAX_SAMPLES);
    //
    //        float variance = calculateVariance(sensorReadings, MAX_SAMPLES);
    //
    //        Serial.print("Variance for sensor ");
    //        Serial.print(sensor_num);
    //        Serial.print(": ");
    //        Serial.println(variance);
    //
    //      }
    //
    //    }

    // Normalize a set of readings to be between 0 and 1
    //    void normalizeReadings(unsigned long readings[], int numReadings) {
    //
    //      unsigned long minVal = readings[0];
    //      unsigned long maxVal = readings[0];
    //
    //      // Find the min and max values
    //      for (int i = 1; i < numReadings; i++) {
    //
    //        if (readings[i] < minVal) {
    //          minVal = readings[i];
    //        }
    //
    //        if (readings[i] > maxVal) {
    //          maxVal = readings[i];
    //        }
    //
    //      }
    //
    //      // Normalize the readings
    //      for (int i = 0; i < numReadings; i++) {
    //        readings[i] = (float)(readings[i] - minVal) / (maxVal - minVal);
    //      }
    //
    //    }

    // Calculate the variance of a set of normalized readings
    // Paul: I think we can save a bit of memory by collecting
    //       our samples within this function.
    float calculateVariance( ) {

      unsigned long start_time = millis();

      // 5 sensors, 10 samples each
      int num_samples = 10;
      float samples[5][ num_samples ];

      float sum[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
      float mean[5] = {0.0, 0.0, 0.0, 0.0, 0.0};


      // Collect samples
      for ( int sample = 0; sample < num_samples; sample++ ) {

        getCalibrated();

        // Copy for each sensor into samples array
        for ( int sensor = 0; sensor < 5; sensor++ ) {
          samples[ sensor ][ sample ] = calibrated[ sensor ];

          // Might as well capture sum whilst were here
          sum[ sensor ] += calibrated[ sensor ];
        }

      }

      // Calculate means
      for( int sensor = 0; sensor < 5; sensor++ ) {
        mean[sensor] = sum[sensor] / (float)num_samples;
      }
      
 
      for( int sensor = 0; sensor < 5; sensor++ ) {
          
          variance[ sensor ] = 0.0;

          for( int sample = 0; sample < num_samples; sample++ ) {
            variance[sensor] += pow( samples[sensor][sample] - mean[sensor], 2);
          }
          variance[sensor] /= (float)num_samples;

      }

      unsigned long end_time = millis();

      // Paul: I found this takes between 10-30ms depending on
      //       the surface tone, which is interesting.
      //Serial.print("Digital Variance calculation took: ");
      //Serial.println( (int)( end_time - start_time ) );
    
    }

};

#endif

