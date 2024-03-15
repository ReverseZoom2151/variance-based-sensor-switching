#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN 9
#define R_DIR_PIN 15
#define EMIT_PIN 11
#define FWD LOW
#define REV HIGH
#define BUZZER_PIN 6
#define state_init 0
#define state_joinline 1
#define state_followline 2
#define state_intersection 3
#define state_finish 4

int state;


void setup() {
  pinMode(10, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(A2, INPUT_PULLUP);   //line sensor 3
  pinMode(A0, INPUT_PULLUP);   //line sensor 2
  pinMode(A3, INPUT_PULLUP);   //line sensor 4
  pinMode(A11, INPUT_PULLUP);  // line sensor 1 (LEFTMOST)
  pinMode(A4, INPUT_PULLUP);  // line sensor 5 (RIGHTMOst)
  

  pinMode(EMIT_PIN, OUTPUT);
  digitalWrite(EMIT_PIN, HIGH);

  //initialising motor
  digitalWrite(10, 0);
  digitalWrite(9, 0);
  //initialising direction
  digitalWrite(16, LOW);
  digitalWrite(15, LOW);

  // Start serial, send debug text.
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  state = state_init;
  
}
void beep() {
  digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
  delay(100); // Beep for 100 milliseconds
  digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
}

void blink(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  }
void blinkinter(){
  for (int i = 0; i < 5; i++){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
}
int lineCounter = 0;

const int threshold = 860;
const int w_thresh1 = 800;
const int w_thresh5 = 750;
const int w_thresh24 = 700;
const int w_thresh3 = 620;
int dn3;
int dn2;
int dn4;
int dn1;
int dn5;
int left_motor_speed = 20;
int right_motor_speed = 20;
unsigned long startFollowTime = 0;
int turn180Count = 0;
int turnLcount = 0;
int r90 = 0;
int l90 = 0;
bool left2right = false;

void updateState() {
  switch (state) {
    case state_init:
      state = state_joinline;
      beep();
      blinkinter();
      Serial.println("INIT -> JOIN");
      break;
      
    case state_joinline:
      if (dn3 > threshold && dn2 > threshold && dn4 > threshold) {
        if (dn1 > threshold && dn5 > threshold){
        //line detected
        lineCounter++;
      }
      }
    

      if (lineCounter == 2) {
        state = state_followline;
        if (startFollowTime == 0){
          startFollowTime = millis();
        }
        
        Serial.println("State changed: joinline -> followline");
        break;
      }
      break;
      
      case state_followline:

      if (millis() - startFollowTime >= 8000){
      if (dn2 > threshold && dn3 > threshold && dn4 > threshold){
        //  
        state = state_intersection;
        beep();
        //blink();
        startFollowTime = 0;
        
        Serial.println("State changed: follow -> intersection");
        
      }
      }
      break;
      
      case state_intersection:
      if (l90 >= 1 && r90 >= 1){
        left2right = true;
      }
      if (left2right && turn180Count == 5 && turnLcount >=52){
        state = state_finish;
        blink();
        }
          if (!left2right && turn180Count == 2 && turnLcount >=18){
        state = state_finish;
        beep();
        blink();
      }
        
  
    break;

      
      case state_finish:
      break;
    }
}


  





void loop() {
  digitalWrite(16, LOW);
  digitalWrite(15, LOW);
  analogWrite(10, 20);
  analogWrite(9, 20);

  dn3 = analogRead(A2);
  dn2 = analogRead(A0);
  dn4 = analogRead(A3);
  dn1 = analogRead(A11);
  dn5 = analogRead(A4);



  // Serial.print(dn1);
  // Serial.print(",");
  // Serial.print(dn2);
  // Serial.print(",");
  // Serial.print(dn3);
  // Serial.print(",");
  // Serial.print(dn4);
  // Serial.print(",");
  // Serial.print(dn5);
  // Serial.print("\n");
  // Serial.print(turnCount);

  updateState();
  switch (state) {
    case state_init:
    
      break;
    case state_joinline:
    forward();
      break;
    case state_followline:
      // Straight line following
      if (dn3 > threshold) {
         forward();
      }
      
      else if (dn2 > threshold) {// Left turn detected
        left45();    
      }                                              
      else if (dn4 > threshold) { //right turn
        right45();
      }  
      else if (dn5 > threshold) { //90 degree turn right
        right90();
        blink();
        r90++;

      }  
      else if (dn1 > threshold) { //90 degree turn left
        left90();
        blink();
        l90++;
      }  
      else if (dn1 < w_thresh1 && dn2 < w_thresh24 && dn3 < w_thresh3 && dn4 < w_thresh24 && dn5 < w_thresh5) { //end of line
        turn180();
        delay(100);
      }
      delay(100);

      break;
      case state_intersection:
       if (dn3 > threshold && dn1 < w_thresh1 && dn2 < w_thresh24 && dn4 < w_thresh24 && dn5 < w_thresh5) {
         forward();
      }
      else if (dn2 > threshold  ) {// Left turn detected
        interleft();
        //blink();
        turnLcount++;
      }                                         
      else if (dn1 > threshold) { //90 degree turn left
        left90();
      }  
      else if (dn1 < w_thresh1 && dn2 < w_thresh24 && dn3 < w_thresh3 && dn4 < w_thresh24 && dn5 < w_thresh5) { //end of line
        turn180();
        turn180Count++;
      }
      delay(100);
      break;
      case state_finish:
       if (dn3 > threshold) {
         forward();
      }
      
      else if (dn2 > threshold) {// Left turn detected
        left45();    
      }                                              
      else if (dn4 > threshold) { //right turn
        rightFinish();
      }  
      else if (dn5 > threshold) { //90 degree turn right
        right90();
      }  
      else if (dn1 > threshold) { //90 degree turn left
        left90();
      } 
      else if (dn3<w_thresh3 && dn2 <w_thresh24 && dn4 < w_thresh24){
        stopRobot();
      }
      delay(100);
      break;
      delay(200);
  }
}



void stopRobot() {
  analogWrite(L_PWM_PIN, 0);
  analogWrite(R_PWM_PIN, 0);
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, LOW);
}

void forward(){
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, LOW);
  analogWrite(L_PWM_PIN, left_motor_speed);
  analogWrite(R_PWM_PIN, right_motor_speed);
}

void left45(){
  digitalWrite(L_DIR_PIN, HIGH);
  digitalWrite(R_DIR_PIN, LOW); 
  analogWrite(L_PWM_PIN, left_motor_speed/2);  
  analogWrite(R_PWM_PIN, right_motor_speed);     
}

void right45(){
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, HIGH);
  analogWrite(L_PWM_PIN, left_motor_speed);
  analogWrite(R_PWM_PIN, right_motor_speed/2);
}

void right90(){
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, HIGH);
  analogWrite(L_PWM_PIN, left_motor_speed*1.2);
  analogWrite(R_PWM_PIN, right_motor_speed*1.2);
}

void left90(){
  digitalWrite(L_DIR_PIN, HIGH);
  digitalWrite(R_DIR_PIN, LOW);
  analogWrite(L_PWM_PIN, left_motor_speed*1.2);
  analogWrite(R_PWM_PIN, right_motor_speed*1.2);
}
void turn180(){
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, HIGH);
  analogWrite(L_PWM_PIN, left_motor_speed*2);
  analogWrite(R_PWM_PIN, right_motor_speed*2);
  delay(400);
}

void interleft(){
  digitalWrite(L_DIR_PIN, HIGH);
  digitalWrite(R_DIR_PIN, LOW); 
  analogWrite(L_PWM_PIN, left_motor_speed);  
  analogWrite(R_PWM_PIN, right_motor_speed*1.1);     
}

void rightFinish(){
  digitalWrite(L_DIR_PIN, LOW);
  digitalWrite(R_DIR_PIN, HIGH);
  analogWrite(L_PWM_PIN, left_motor_speed*1.2);
  analogWrite(R_PWM_PIN, right_motor_speed*0.8);
}






