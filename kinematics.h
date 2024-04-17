// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _KINEMATICS_H
#define _KINEMATICS_H

// class to track robot position.
class Kinematics_c {

  private:

    long lastLeftEncoderCount = 0; // last encoder count for the left wheel
    long lastRightEncoderCount = 0; // last encoder count for the right wheel

    // Paul: Pololu 3Pi+ parameters - these can be calibrated
    const float wheelRadius = 16.5; // wheel diameter in mm
    const float wheel_separation = 42.5; // distance between wheels in meters
    const float encoderCountsPerRevolution = 358.3; // encoder counts per wheel revolution
    const float mmPerCount = (2.0 * PI * wheelRadius) / encoderCountsPerRevolution; // mm traveled per encoder count

    int thetaLogSize = 100;
    float thetaLog[50];     // Array to store theta values
    

  public:

    // Paul: moved these to public area so we can access them
    //       from loop() by using the . operator (e.g. kinematics.xPos)
    float xPos = 0; // X position of the robot in the global frame
    float yPos = 0; // Y position (remains 0 in the initial implementation)
    float theta = 0; // robot's orientation in radians
    int currentIndex = 0; 

    Kinematics_c() {

    }

    void printTheta() {
        for (int i = 0; i < currentIndex; i++) {
            Serial.print(thetaLog[i]);
            Serial.print(",");

        }
       Serial.print("\n");

    }


    // Paul: this simple function should be working...
    // function used to update kinematics
    void update(long leftEncoderCount, long rightEncoderCount) {

      // calculates change in encoder counts since last update
      long deltaLeft = leftEncoderCount - lastLeftEncoderCount;
      long deltaRight = rightEncoderCount - lastRightEncoderCount;

      // updates last encoder count for next iteration(s)
      lastLeftEncoderCount = leftEncoderCount;
      lastRightEncoderCount = rightEncoderCount;

      // calculates distance moved by each wheel
      float distanceLeft = deltaLeft * mmPerCount;
      float distanceRight = deltaRight * mmPerCount;

      // calculates average distance moved (X_R')
      float distanceAverage = (distanceLeft + distanceRight) / 2.0;

      // updates global X position (assuming no change in Y position for simplicity)
      xPos += distanceAverage * cos(theta);
      yPos += distanceAverage * sin(theta);

      // calculates change in orientation (θ_R')
      float deltaTheta = (distanceRight - distanceLeft) / (wheel_separation * 2.0);
      theta += deltaTheta;

      thetaLog[currentIndex] = theta;
        currentIndex++;

      if (currentIndex >= thetaLogSize) {
          currentIndex = 0; // Reset index for circular buffer behavior
        }

    }

};

#endif
