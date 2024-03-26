
#include "analoglinesensors.h"
#include "digitallinesensors.h"

#include "motors.h"

AnalogLineSensors_c   a_sensors;
DigitalLineSensors_c  d_sensors;

// For motor control
Motors_c motors;

// variables for storing results
unsigned long update_ts;
#define MAX_RESULTS 100
float results[ MAX_RESULTS ];
int results_index;

int state;
#define STATE_RUNNING_TRIAL 0
#define STATE_PRINT_RESULTS 1


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
  motors.setMotorsPWM( -80, 80 ); // start spinning
  a_sensors.calibrate();
  d_sensors.calibrate();

  // Stop spinning!
  motors.setMotorsPWM( 0, 0 );


  // Some beeping + delay so you can move it to the start location
  int count = 0;
  while ( count < 10 ) {

    pinMode( 6, OUTPUT );
    analogWrite(6, 120 );
    delay(5);
    analogWrite(6, 0 );
    delay(500);
    count++;
  }


  // Paul: debugging
  //        testing both analog and digital can be done
  //        one after the other, seems to work :)
  //        You can uncomment each set to see what it is
  //        doing.
  //while ( true ) {

  //a_sensors.getCalibrated();
  //a_sensors.printCalibrated();

  //d_sensors.getCalibrated();
  // d_sensors.printCalibrated();

  //d_sensors.calculateVariance();
  //d_sensors.printVariance();

  //a_sensors.calculateVariance();
  //a_sensors.printVariance();


  //}


  // Prepare to start.
  update_ts = millis();
  results_index = 0;
  state = STATE_RUNNING_TRIAL;

}


void loop() {

  if ( state == STATE_RUNNING_TRIAL ) {
    // Every 100ms...
    if ( millis() - update_ts > 100 ) {
      update_ts = millis();

      // Some behaviuor code here?
      // ...
      // ...
      d_sensors.getCalibrated();

      if ( results_index < MAX_RESULTS ) {

        // Just as an example.
        // This could be any other variable from your
        // system.
        results[ results_index ] = (float)millis();

        // incrment for next time.
        results_index++;

      } else {
        // filled up results array? experiment over.
        state = STATE_PRINT_RESULTS;

        // Might as well stop the robot
        motors.setMotorsPWM( 0, 0 );

        // Beep to signal end
        pinMode( 6, OUTPUT );
        analogWrite(6, 120 );
        delay(5);
        analogWrite(6, 0 );

      }

    }
  } else if ( state == STATE_PRINT_RESULTS ) {

    // Just print the results, and give a delay so
    // you have time to copy-pasta
    for ( int i = 0; i < MAX_RESULTS; i++ ) {
      Serial.print( results[i] );
      Serial.print(",");
    }
    Serial.print("\n\n\n");

    delay( 3000 );

  }

}
