// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _KINEMATICS_H
#define _KINEMATICS_H
#define PI 3.1415926535897932384626433832795

// class to track robot position.
class Kinematics_c {

  private:

    float xPos = 0; // X position of the robot in the global frame
    float yPos = 0; // Y position (remains 0 in the initial implementation)
    float theta = 0; // robot's orientation in radians

    long lastLeftEncoderCount = 0; // last encoder count for the left wheel
    long lastRightEncoderCount = 0; // last encoder count for the right wheel

    const float wheelRadius = 0.035; // wheel diameter in meters
    const float wheelDiameter = 0.070; // wheel diameter in meters
    const float wheelDistance = 0.140; // distance between wheels in meters 
    const float encoderCountsPerRevolution = 1440.0; // encoder counts per wheel revolution
    const float mmPerCount = (2 * PI * wheelRadius) / encoderCountsPerRevolution; // mm traveled per encoder count

  public:
  
    Kinematics_c() {

    } 

    // // function used to update kinematics
    // void update(long leftEncoderCount, long rightEncoderCount) {

    //   // calculates change in encoder counts since last update 
    //   long deltaLeft = leftEncoderCount - lastLeftEncoderCount;
    //   long deltaRight = rightEncoderCount - lastRightEncoderCount;

    //   // updates last encoder count for next iteration(s)
    //   lastLeftEncoderCount = leftEncoderCount;
    //   lastRightEncoderCount = rightEncoderCount;

    //   // calculates distance moved by each wheel 
    //   float distanceLeft = deltaLeft * mmPerCount;
    //   float distanceRight = deltaRight * mmPerCount;

    //   // calculates average distance moved (X_R')
    //   float distanceAverage = (distanceLeft + distanceRight) / 2.0;

    //   // updates global X position (assuming no change in Y position for simplicity)
    //   xPos += distanceAverage * cos(theta);
    //   yPos += distanceAverage * sin(theta);

    //   // calculates change in orientation (θ_R')
    //   float deltaTheta = (distanceRight - distanceLeft) / wheelDistance;
    //   theta += deltaTheta;

    //   // ensures theta stays within a sensible range, -π to π
    //   if (theta > PI) {
    //     theta -= 2 * PI;
    //   } else if (theta < -PI) {
    //     theta += 2 * PI;
    //   }

    // }

    void update(long leftEncoderCount, long rightEncoderCount) {

        long deltaLeft = leftEncoderCount - lastLeftEncoderCount;
        long deltaRight = rightEncoderCount - lastRightEncoderCount;

        lastLeftEncoderCount = leftEncoderCount;
        lastRightEncoderCount = rightEncoderCount;

        float distanceLeft = deltaLeft * mmPerCount;
        float distanceRight = deltaRight * mmPerCount;
        float deltaTheta = (distanceRight - distanceLeft) / wheelDistance;

        if (deltaLeft == deltaRight) {

          xPos += distanceLeft * cos(theta);
          yPos += distanceLeft * sin(theta);

        } else {

          float R = wheelDistance * (distanceLeft + distanceRight) / (2 * (distanceRight - distanceLeft));
          float ICR_x = xPos - R * sin(theta);
          float ICR_y = yPos + R * cos(theta);

          float newX = cos(deltaTheta) * (xPos - ICR_x) - sin(deltaTheta) * (yPos - ICR_y) + ICR_x;
          float newY = sin(deltaTheta) * (xPos - ICR_x) + cos(deltaTheta) * (yPos - ICR_y) + ICR_y;

          xPos = newX;
          yPos = newY;
          theta += deltaTheta;

        }

        // Normalize theta to stay within [-π, π]
        while (theta > PI) theta -= 2 * PI;
        while (theta < -PI) theta += 2 * PI;
    }

    // calculates angle relative to the home position
    float calculateAngleToHome() const {

      // home is at (0,0) and angle is measured from the positive X-axis
      float angleToHomeRad = atan2(-yPos, -xPos); // angle from current position back to home
      // converts to degrees and adjust range to [0,360)
      float angleToHomeDeg = (angleToHomeRad * 180 / PI) + 180;
      return angleToHomeDeg;
    
    }

    float calculateDistanceHome() const {

      // euclidean distance from current position to home (0,0)
      return sqrt(xPos * xPos + yPos * yPos);

    }

    float getOrientationDegrees() const {

      // convert radians to degrees 
      return theta * 180 / PI;

    }

};

#endif
