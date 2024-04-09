#include "analoglinesensors.h"
#include "digitallinesensors.h"
#include "motors.h"

#define MAX_RESULTS 100
#define STATE_RUNNING_TRIAL 0
#define STATE_PRINT_RESULTS 1
#define SENSOR_THRESHOLD
#define ANALOG_THRESHOLD 0.5
#define DIGITAL_THRESHOLD 0.619

AnalogLineSensors_c a_sensors;
DigitalLineSensors_c d_sensors;

// For motor control
Motors_c motors;

// variables for storing results
unsigned long update_ts;
float results[MAX_RESULTS];
int results_index;
int state;
const float BiasPWM = 30.0; // starts with a positive forward bias
const float MaxTurnPWM = 20.0; 

bool digitalLineDetected() {

  d_sensors.getCalibrated();

  unsigned long digitalLeftSensorReading = d_sensors.calibrated[1]; // DN2
  unsigned long digitalMiddleSensorReading = d_sensors.calibrated[2]; // DN3
  unsigned long digitalRightSensorReading = d_sensors.calibrated[3]; // DN4

  // reads all three line sensors through the digital method and determines if any sensor detects the line
  return ((digitalLeftSensorReading >= DIGITAL_THRESHOLD) || (digitalMiddleSensorReading >= DIGITAL_THRESHOLD) || (digitalRightSensorReading >= DIGITAL_THRESHOLD));

}

bool analogLineDetected() {

  a_sensors.getCalibrated(); 

  unsigned long analogLeftSensorReading = a_sensors.calibrated[1]; // DN2
  unsigned long analogMiddleSensorReading = a_sensors.calibrated[2]; // DN3
  unsigned long analogRightSensorReading = a_sensors.calibrated[3]; // DN4

  // reads all three line sensors through the analog method and determines if any sensor detects the line
  return ((analogLeftSensorReading >= ANALOG_THRESHOLD) || (analogMiddleSensorReading >= ANALOG_THRESHOLD) || (analogRightSensorReading >= ANALOG_THRESHOLD));

}

float analogWeightedMeasurement() {

  a_sensors.getCalibrated(); 

  unsigned long analogLeftSensorReading = a_sensors.calibrated[1]; // DN2
  unsigned long analogRightSensorReading = a_sensors.calibrated[3]; // DN4
  unsigned long sum = analogLeftSensorReading + analogRightSensorReading;

  float analogLeftNormalized = (float)analogLeftSensorReading / sum;
  float analogRightNormalized = (float)analogRightSensorReading / sum;
  float analogLeftWeighted = 2.0 * analogLeftNormalized;
  float analogRightWeighted = 2.0 * analogRightNormalized;
  float W = analogRightWeighted - analogLeftWeighted;
  // float W = analogLeftWeighted - analogRightWeighted;

  // Serial.print("Analog Weighted Measurement (W): ");
  // Serial.println(W); 

  return W;

}

float digitalWeightedMeasurement() {

  d_sensors.getCalibrated(); 

  unsigned long digitalLeftSensorReading = d_sensors.calibrated[1]; // DN2
  unsigned long digitalRightSensorReading = d_sensors.calibrated[3]; // DN4
  unsigned long sum = digitalLeftSensorReading + digitalRightSensorReading;

  float digitalLeftNormalized = (float)digitalLeftSensorReading / sum;
  float digitalRightNormalized = (float)digitalRightSensorReading / sum;
  float digitalLeftWeighted = 2.0 * digitalLeftNormalized;
  float digitalRightWeighted = 2.0 * digitalRightNormalized;
  float W = digitalRightWeighted - digitalLeftWeighted;
  // float W = digitalLeftWeighted - digitalRightWeighted;

  // Serial.print("Digital Weighted Measurement (W): ");
  // Serial.println(W); 

  return W;

}

void analogFollowLine() {

  a_sensors.getCalibrated(); 

  unsigned long analogFarLeftSensorReading = a_sensors.calibrated[0]; // DN1
  unsigned long analogLeftSensorReading = a_sensors.calibrated[1]; // DN2
  unsigned long analogMiddleSensorReading = a_sensors.calibrated[2]; // DN3
  unsigned long analogRightSensorReading = a_sensors.calibrated[3]; // DN4
  unsigned long analogFarRightSensorReading = a_sensors.calibrated[4]; // DN5

  if (analogLineDetected()) { 

    float W = analogWeightedMeasurement();
    float LeftPWM = BiasPWM + (MaxTurnPWM * W);
    float RightPWM = BiasPWM - (MaxTurnPWM * W);
    
    motors.setMotorsPWM(LeftPWM, RightPWM);  
  
  }
  
  if (analogFarLeftSensorReading >= ANALOG_THRESHOLD) {

    // executes a sharp left turn for corners (90 degrees)
    motors.spinLeft(BiasPWM, 250);
  
  } else if (analogFarRightSensorReading >= ANALOG_THRESHOLD) {

    // executes a sharp left turn for corners (90 degrees)
    motors.spinRight(BiasPWM, 250);
  
  }

}

void digitalFollowLine() {

  d_sensors.getCalibrated(); 

  unsigned long digitalFarLeftSensorReading = d_sensors.calibrated[0]; // DN1
  unsigned long digitalLeftSensorReading = d_sensors.calibrated[1]; // DN2
  unsigned long digitalMiddleSensorReading = d_sensors.calibrated[2]; // DN3
  unsigned long digitalRightSensorReading = d_sensors.calibrated[3]; // DN4
  unsigned long digitalFarRightSensorReading = d_sensors.calibrated[4]; // DN5

  if (digitalLineDetected()) { 

    float W = digitalWeightedMeasurement();
    float LeftPWM = BiasPWM + (MaxTurnPWM * W);
    float RightPWM = BiasPWM - (MaxTurnPWM * W);
    
    motors.setMotorsPWM(LeftPWM, RightPWM);  
  
  }

  if (digitalFarLeftSensorReading >= DIGITAL_THRESHOLD) {

    // executes a sharp left turn for corners (90 degrees)
    motors.spinLeft(BiasPWM, 250);
  
  } else if (digitalFarRightSensorReading >= DIGITAL_THRESHOLD) {

    // executes a sharp left turn for corners (90 degrees)
    motors.spinRight(BiasPWM, 250);
  
  }

}

void setup() {
  
  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(2000);
  Serial.println("***RESET***");

  // To calibrate, we set the robot spinning on the spot
  // briefly, and call the calibration routine.
  // calibrate() is blocking, so it will take the time it
  // needs.
  // Place the robot so the sensors will sweep over
  // black and white surfaces.
  motors.setMotorsPWM(-80, 80); // start spinning
  a_sensors.calibrate();
  // d_sensors.calibrate();

  // Stop spinning!
  motors.setMotorsPWM(0, 0);

  // Some beeping + delay so you can move it to the start location
  int count = 0;
  
  while (count < 10) {

    pinMode(6, OUTPUT);
    analogWrite(6, 120);
    delay(5);
    analogWrite(6, 0);
    delay(500);
    count++;
    
  }

  // Paul: debugging
  //        testing both analog and digital can be done
  //        one after the other, seems to work :)
  //        You can uncomment each set to see what it is
  //        doing.
  // while (true) {

    // a_sensors.getCalibrated();
    // a_sensors.printCalibrated();
    // a_sensors.calculateVariance();
    // a_sensors.printVariance();

    // d_sensors.getCalibrated();
    // d_sensors.printCalibrated();
    // d_sensors.calculateVariance();
    // d_sensors.printVariance();

  // }

  // Prepare to start.
  update_ts = millis();
  results_index = 0;
  state = STATE_RUNNING_TRIAL;

}

void loop() {

  if (state == STATE_RUNNING_TRIAL) {
    
    // Every 100ms...
    if (millis() - update_ts > 100) {
      
      update_ts = millis();

      a_sensors.getCalibrated();
      d_sensors.getCalibrated();

      // Serial.print("Digital:");
      // d_sensors.printCalibrated();
      // a_sensors.calculateVariance();
      // d_sensors.calculateVariance();
      analogVariance = a_sensors.calculateAverageVariance();
      digitalVariance = d_sensors.calculateAverageVariance();
      // // Serial.print("analogVariance:");
      // // a_sensors.printVariance();
      // // Serial.print("DigitalVariance:");
      // // d_sensors.printVariance();

      if (analogVariance < digitalVariance) {
        analogFollowLine();
      } else {
        digitalFollowLine();
        }
      // analogFollowLine();
      //digitalFollowLine();


      if (results_index < MAX_RESULTS) {

        // Just as an example.
        // This could be any other variable from your system.
        results[results_index] = (float)millis();

        // increment for next time.
        results_index++;

      } else {
        
        // filled up results array? experiment over.
        // state = STATE_PRINT_RESULTS;

        // Might as well stop the robot
        // motors.setMotorsPWM(0, 0);

        // // Beep to signal end
        // pinMode(6, OUTPUT);
        // analogWrite(6, 120);
        // delay(5);
        // analogWrite(6, 0); 

      }

    }
    
  } else if (state == STATE_PRINT_RESULTS) {

    // Just print the results, and give a delay so
    // you have time to copy-paste

    for (int i = 0; i < MAX_RESULTS; i++) {

      Serial.print(results[i]);
      Serial.print(",");
    
    }
    
    Serial.print("\n\n\n");

    delay(3000);

  }

}
