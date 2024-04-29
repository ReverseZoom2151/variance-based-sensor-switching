#include "analoglinesensors.h"
#include "digitallinesensors.h"
#include "motors.h"
#include "encoders.h"
#include "kinematics.h"

#define MAX_RESULTS 80
#define STATE_RUNNING_TRIAL 0
#define STATE_PRINT_RESULTS 1
#define SENSOR_THRESHOLD
#define ANALOG_THRESHOLD 0.5
#define DIGITAL_THRESHOLD 0.619

AnalogLineSensors_c a_sensors;
DigitalLineSensors_c d_sensors;
Kinematics_c kinematics;
Motors_c motors;

// variables for storing results
unsigned long update_ts;
float results[MAX_RESULTS];
int results_index;
int state;
const float BiasPWM = 30.0; // starts with a positive forward bias
const float MaxTurnPWM = 20.0;

float analogWeightedMeasurement() {

  a_sensors.getCalibrated();

  // Paul: needs to be a float. You can also acccess
  //       the variables in a class (a_sensors) directly
  //       with the dot operator.
  float sum = a_sensors.calibrated[1] + a_sensors.calibrated[3];

  if (sum == 0.0) return 0.0;

  float analogLeftWeighted = (float)a_sensors.calibrated[1] / sum;
  float analogRightWeighted = (float)a_sensors.calibrated[3] / sum;
  float W = analogRightWeighted - analogLeftWeighted;

  // Serial.print("Analog Weighted Measurement (W): ");
  // Serial.println(W);

  return W;

}

float digitalWeightedMeasurement() {

  d_sensors.getCalibrated();

  // Paul: needs to be a float. You can also acccess
  //       the variables in a class (a_sensors) directly
  //       with the dot operator.
  float sum = d_sensors.calibrated[1] + d_sensors.calibrated[3];

  if (sum == 0.0) return 0.0;

  float digitalLeftWeighted = (float)d_sensors.calibrated[1] / sum;
  float digitalRightWeighted = (float)d_sensors.calibrated[3] / sum;
  float W = digitalRightWeighted - digitalLeftWeighted;

  // Serial.print("Digital weighted measurement (W): ");
  // Serial.println(W);

  return W;

}

void analogFollowLine() {

  a_sensors.getCalibrated();
  a_sensors.calculateVariance();
  

  kinematics.update(count_e0, count_e1);
  kinematics.printTheta();

  // Serial.println();

  // kinematics.printTheta();
  // Serial.println();
  


  float W = analogWeightedMeasurement();
  float LeftPWM = BiasPWM + (MaxTurnPWM * W);
  float RightPWM = BiasPWM - (MaxTurnPWM * W);

  motors.setMotorsPWM(LeftPWM, RightPWM);
  float sum = a_sensors.calibrated[1] + a_sensors.calibrated[3];
  if (sum < 0.4) {
    motors.setMotorsPWM(0, 0);
    for (int i = 0; i < 5; i++) {  // Assuming a_sensors.variance has 5 elements
        a_sensors.variance[i] = 0.0;
  }
  
  }
}

void digitalFollowLine() {

  d_sensors.getCalibrated();
  d_sensors.calculateVariance();
  kinematics.update(count_e0, count_e1);
  kinematics.printTheta();


  float W = digitalWeightedMeasurement();
  float LeftPWM = BiasPWM + (MaxTurnPWM * W);
  float RightPWM = BiasPWM - (MaxTurnPWM * W);

  motors.setMotorsPWM(LeftPWM, RightPWM);
  float sum = d_sensors.calibrated[1] + d_sensors.calibrated[3];
  if (sum < 0.1) {
    motors.setMotorsPWM(0, 0);
    for (int i = 0; i < 5; i++) {  // Assuming a_sensors.variance has 5 elements
        d_sensors.variance[i] = 0.0;
  }
  }
}

void lineFollowing() {
    // Calculate variances
    a_sensors.getCalibrated();
    d_sensors.getCalibrated();
    a_sensors.calculateVariance();
    d_sensors.calculateVariance();
    // a_sensors.printVariance();
    //d_sensors.printVariance();
    kinematics.update(count_e0, count_e1);
    kinematics.printTheta();

    // Determine the preferred measurement method 
    float analogVariance = a_sensors.calculateAverageVariance();
    float digitalVariance = d_sensors.calculateAverageVariance();

    float W;
    if (analogVariance < digitalVariance) {
        // Use analog method
        a_sensors.getCalibrated(); // Get updated calibrated values
        W = analogWeightedMeasurement();
    //     Serial.print("AnalogVariance: ");
    //     Serial.print("\n");
    //     a_sensors.printVariance();
    } else {
        // Use digital method
        d_sensors.getCalibrated(); // Get updated calibrated values
        W = digitalWeightedMeasurement();
        // Serial.print("DigitalVariance: ");
        // Serial.print("\n");
        // d_sensors.printVariance();
    }

    // Calculate motor PWM values
    float LeftPWM = BiasPWM + (MaxTurnPWM * W);
    float RightPWM = BiasPWM - (MaxTurnPWM * W);

    // Apply motor commands
    motors.setMotorsPWM(LeftPWM, RightPWM);
    float a_sum = a_sensors.calibrated[1] + a_sensors.calibrated[3];
    float d_sum = d_sensors.calibrated[1] + d_sensors.calibrated[3];
  if (a_sum < 0.4 || d_sum < 0.1) {
    motors.setMotorsPWM(0, 0);
  } 
}

void setup() {

  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(2000);
  Serial.println("***RESET***");

  // Paul: setup encoders ready to use
  setupEncoder0();
  setupEncoder1();

  // To calibrate, we set the robot spinning on the spot briefly, and call the calibration routine.
  // calibrate() is blocking, so it will take the time it needs.
  // Place the robot so the sensors will sweep over black and white surfaces.
  motors.setMotorsPWM(-80, 80); // start spinning
  a_sensors.calibrate();
  //d_sensors.calibrate();

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
  //       testing both analog and digital can be done one after the other, seems to work :)
  //       You can uncomment each set to see what it is doing.
   while (true) {

//   //   // Here, we still need to call getCalibrated()
//   //   // and calculateVariance().

     a_sensors.getCalibrated();
     a_sensors.printCalibrated();
     a_sensors.calculateVariance();
    //  a_sensors.printVariance();
    a_sensors.calculateAverageVariance();

  //  d_sensors.getCalibrated();
    //d_sensors.printCalibrated();
//   //   // d_sensors.calculateVariance();
//   //   // d_sensors.printVariance();

//   //   // Paul: I was using the following 4 lines to look at
//   //   //       any difference between the weighted measurement
//   //   //       for digital vs analog
//   //   // Serial.print(digitalWeightedMeasurement() * 10.0);
//   //   // Serial.print(",");
//   //   // Serial.print(analogWeightedMeasurement() * 10.0);
//   //   // Serial.print("\n");

//   //   // Paul: I was using the following 4 lines to look at
//   //   //       any difference in variance between analog vs
//   //   //       digital, but only for sensor 2 (middle front)
//   //   // Serial.print(a_sensors.variance[2] * 10000.0);
//   //   // Serial.print(",");
//   //   // Serial.print(d_sensors.variance[2] * 10000.0);
//   //   // Serial.print("\n");

//   //   // Paul: I was using the below 4 lines to quickly check that
//   //   //       the encoders were both going up/down in a consistent
//   //   //       and sensible way when I push the robot by hand.
//   //   // Serial.print(count_e0);
//   //   // Serial.print(",");
//   //   // Serial.print(count_e1);
//   //   // Serial.print("\n");

//   //   // Paul: I used the following lines to test the kinematics
//   //   //       was working ok.  Note that, in loop(), you'll also
//   //   //       want to call kinematics.update() regularly. I 
//   //   //       recommend you call it every 20ms or something,
//   //   //       not just every time loop() happens.
    // kinematics.update(count_e0, count_e1);
    // kinematics.printTheta();
//   //   Serial.print(kinematics.xPos);
//   //   Serial.print(",");
//   //   Serial.print(kinematics.yPos);    
//   //   Serial.print(",");
//   //   Serial.print(kinematics.theta);
//   //   Serial.print("\n");

     delay(10);

    }

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

      // a_sensors.getCalibrated();
     // kinematics.update(count_e0, count_e1);
      // kinematics.printTheta();
      //analogFollowLine();
     // a_sensors.calculateVariance();
      //a_sensors.printVariance();

      // d_sensors.getCalibrated();
      //digitalFollowLine();
      
      lineFollowing();
      
      if (results_index < MAX_RESULTS) {

        // Just as an example.
        // This could be any other variable from your system.
        results[results_index] = (float)millis();

        // increment for next time.
        results_index++;

      } else {

        //filled up results array? experiment over.
        state = STATE_PRINT_RESULTS;

        //Might as well stop the robot
        motors.setMotorsPWM(0, 0);

        // Beep to signal end
        pinMode(6, OUTPUT);
        analogWrite(6, 120);
        delay(5);
        analogWrite(6, 0);

      }

    }

  // } else if (state == STATE_PRINT_RESULTS) {

  //   // Just print the results, and give a delay so
  //   // you have time to copy-paste
  //   for (int i = 0; i < MAX_RESULTS; i++) {

  //     Serial.print(results[i]);
  //     Serial.print(",");

  //   }

  //   Serial.print("\n\n\n");
  //   delay(3000);

  // }

}
}
