#ifndef ANALOG_H 
#define ANALOG_H 
#define MAX_SAMPLES 10
#define EMIT_PIN 11
#define LS_LEFT_PIN A11  
#define LS_MIDLEFT_PIN A0   
#define LS_MIDDLE_PIN A2   
#define LS_MIDRIGHT_PIN A3  
#define LS_RIGHT_PIN A4  

class Analog_c {

   private:
   
       float readings[5][MAX_SAMPLES]; 
       float normalizedReadings[5][MAX_SAMPLES]; 
       float minReading[5], maxReading[5]; 
       float variance; 
       int ls_pins[5] = {A11, A0, A2, A3, A4};

   public:
   
      Analog_c() {};
   
      void collectReadings(int sensor) {
         if (sensor < 0 || sensor >= 5) {
            Serial.println("Error: Invalid sensor number");
            return;
         }
         
          for (int sample = 0; sample < MAX_SAMPLES; sample++) {
             
              for (int sensor = 0; sensor < 5; sensor++) {
                 
                  float reading = analogRead(ls_pins[sensor]);
                  readings[sensor][sample] = reading;
      
                  // Update min and max
                  if (reading < minReading[sensor]) {
                      minReading[sensor] = reading;
                  }
                 
                  if (reading > maxReading[sensor]) {
                      maxReading[sensor] = reading;
                  }
                 
              }
             
              delay(10); // Short delay between samples (adjust if needed)
             return reading;
             
          }
         
      }
   
      void normalizeReadings(int sensor) {
         if (sensor < 0 || sensor >= 5) {
            Serial.println("Error: Invalid sensor number");
            return;
         }
         
          for (int sensor = 0; sensor < 5; sensor++) {
             
              for (int sample = 0; sample < MAX_SAMPLES; sample++) {
                 
                  normalizedReadings[sensor][sample] = (readings[sensor][sample] - minReading[sensor]) / (maxReading[sensor] - minReading[sensor]);
              
              }
          
          }
         
      }
   
      float calculateVariance(int sensor) {
         if (sensor < 0 || sensor >= 5) {
            Serial.println("Error: Invalid sensor number");
            return;
         }
         
         float sum = 0.0, sumSquare = 0.0, mean, meanSquare, variance;
         
         for (int sample = 0; sample < MAX_SAMPLES; sample++) {
          
           sum += normalizedReadings[sensor][sample];
           sumSquare += normalizedReadings[sensor][sample] * normalizedReadings[sensor][sample];
         
         }
      
          mean = sum / MAX_SAMPLES;
          meanSquare = sumSquare / MAX_SAMPLES;
          variance = meanSquare - (mean * mean);
      
          return variance;
         
       }

void readAnalogSensorsWithVariance() {
    // Collect readings for all sensors (you might optimize this later)
    for (int sensor = 0; sensor < 5; sensor++) {
        collectReadings(sensor); 
    }

    Serial.println("Sensor Readings and Variances:"); // Adjusted header

    for (int sensor = 0; sensor < 5; sensor++) {
        Serial.print("Sensor ");
        Serial.print(sensor);
        Serial.print(": ");

        // Print raw readings (optional)
        // for (int sample = 0; sample < MAX_SAMPLES; sample++) {
        //    Serial.print(readings[sensor][sample]);
        //    Serial.print(" ");
        // }

        normalizeReadings(sensor); // Normalize before variance calculation
        float variance = calculateVariance(sensor); 

        Serial.print("Variance: ");
        Serial.println(variance);
    }
}

};

#endif 


