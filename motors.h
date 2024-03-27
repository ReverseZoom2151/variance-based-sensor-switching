#ifndef _MOTORS_H
#define _MOTORS_H
#define L_PWM 10
#define L_DIR 16
#define R_PWM 9
#define R_DIR 15
#define FWD LOW
#define BCK HIGH

// Class to operate the motor(s).
class Motors_c {

  public:
  
    // Constructor, must exist.
    Motors_c() {
      // leaving this empty.
      // ensure initialise() is called
      // instead.
    } 

    // Use this function to 
    // initialise the pins and 
    // state of your motor(s).
    void initialise() {

      pinMode(L_PWM, OUTPUT);
      pinMode(L_DIR, OUTPUT);
      pinMode(R_PWM, OUTPUT);
      pinMode(R_DIR, OUTPUT);
    
      digitalWrite(L_DIR, HIGH);
      digitalWrite(R_DIR, HIGH);

      // Ensure we init stopped.
      setMotorsPWM(0, 0);

    }

    void setMotorsPWM(float left, float right) {
      
      // Set directions based on sign
      if (left < 0) {

        digitalWrite(L_DIR, BCK);
      
      } else {
        
        digitalWrite(L_DIR, FWD);
      
      }
      
      if (right < 0) {
        
        digitalWrite(R_DIR, BCK);
      
      } else {
       
        digitalWrite(R_DIR, FWD);
      
      }

      // Sign used, now convert to abs 
      // magnitude.
      left = abs(left);
      right = abs(right);

      if (left < 0) {
        
        left = 0;

      }
      
      if (left > 255) {
        
        left = 255;

      }

      if (right < 0) {
        
        right = 0;

      }
      
      if (right > 255) {
        
        right = 255;

      }

      // set power.
      analogWrite(L_PWM, left);
      analogWrite(R_PWM, right);

      // Done!
      return;

    }

    void driveStraight(int speed) {

      // sets directions for forward movement
      digitalWrite(L_DIR, FWD);
      digitalWrite(R_DIR, FWD);

      // drives both motors at the specified speed
      analogWrite(L_PWM, speed); 
      analogWrite(R_PWM, speed);
      
    }

    void spinLeft(const float speed, int pause) {

      digitalWrite(L_DIR, BCK);  
      digitalWrite(R_DIR, FWD);

      analogWrite(L_PWM, speed); 
      analogWrite(R_PWM, speed);

      delay(pause);

    }

    void spinRight(const float speed, int pause) {

      digitalWrite(L_DIR, FWD);  
      digitalWrite(R_DIR, BCK);

      analogWrite(L_PWM, speed); 
      analogWrite(R_PWM, speed);

      delay(pause);

    }
    
};

#endif
