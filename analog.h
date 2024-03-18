#ifndef ANALOG_H 
#define ANALOG_H 
#define MAX_SAMPLES 10

class Analog_c {

public:

   Analog_c() {};

   void collectReadings();  
   void normalizeReadings();     
   float calculateVariance(int sensor);  


private:
    float readings[5][MAX_SAMPLES]; 
    float normalizedReadings[5][MAX_SAMPLES]; 
    float minReading[5], maxReading[5]; 
    float variance; 
    int ls_pins[5] = {A11, A0, A2, A3, A4}; 
};


void Analog_c::collectReadings() {
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
    }
}

void Analog_c::normalizeReadings() {
    for (int sensor = 0; sensor < 5; sensor++) {
        for (int sample = 0; sample < MAX_SAMPLES; sample++) {
            normalizedReadings[sensor][sample] = (readings[sensor][sample] - minReading[sensor]) / (maxReading[sensor] - minReading[sensor]);
        }
    }
}

float Analog_c::calculateVariance(int sensor) {

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

#endif 


