// Define the inbuilt Arduino LED pin
#define ledPin 13

// Input Pin Definitions
#define doorProximitySensor 2 //detect train door in position of PSD // Possibly too hard need think of a better way logically
#define doorPositionLimitSwitchOpen 3 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define doorPositionLimitSwitchClose 4 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define hindranceObstacleDetection 5 //Infared or laser or scales to detect obstacle in oath of door // still figuring out hindrance obstacle detection 
#define emergencyReleaseButton 6 // Emergency stop button if door malfunctions to release the locking mechanism

//pin for fire mode, if off turn everything off flash LEDs

//possible pins for talking to raspberry pi for interface

// Output Pin Definitions
#define motorCWSpin 8 //DC or AC motor to control door movement // 2 Pins to switch the polarity // Clockwise
#define motorACWSpin 9 //DC or AC motor to control door movement // 2 Pins to switch the polarity //Anticlockwise
#define magLockClose 10 //locking device of door // 2 maglocks one for open one for closed
#define magLockOpen 11 //locking device of door // 2 maglocks one for open one for closed
#define audibleSpeaker 12 //alerts of door status
#define doorStatusLightR 14 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Red
#define doorStatusLightG 15 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Green
#define doorStatusLightY 16 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Yellow Amber 
#define systemStatusLED 17 //flashing LED to indicate system is working and running, if not flashing that indication that something is wrong


enum States {
  IDLE,
  TRAIN_APPROACHING,
  DOOR_OPENING,
  DOOR_IS_OPEN,
  DOOR_CLOSING,
  OBSTACLE_DETECTED,
  EMERGENCY_OPEN,
  EMERGENCY_CLOSE
} 

//add global variables here
bool doorOpen = false;
bool doorLocked = true;
bool obstacleDetected = false;
bool trainIsApproaching = false; //this will be changed in the PCU interface, eg. signal from Metro
State currentState = IDLE;

void nextState(State newState) {
  currentState = newState;
}

void StateMachine() {
  switch (currentState)
  {
    case IDLE:
      //Do in this state
        //wait for train to approach with doors shut

      //How to get to next state
        //if alert from system detects train is approaching change to train approaching state
        //if emergency release button is detected 
      if (trainIsApproaching == TRUE){
        nextState(TRAIN_APPROACHING);
      }
      else if (DigitalRead(emergencyReleaseButton) == HIGH){
        nextState(EMERGENCY_OPEN);
      }
      break;

    case TRAIN_APPROACHING:
      //Do in this state
        //Flash Light Red
      flashLight(doorStatusLightL);

      //How to get to next state
        //if proximity sensor is high for train in position
        //and safety interlock determines everything is all safe
        //trigger Door Opening State
      if (DigitalRead(emergencyReleaseButton) == HIGH){
        nextState(EMERGENCY_OPEN);
      }
      else if ((DigitalRead(doorProximitySensor) == HIGH) && (SafetyInterlock() == TRUE)){
        nextState(TRAIN_APPROACHING);
      }
      break;

    case DOOR_OPENING:
      //Do in this state
        //Unlock Maglock Close Pin
        //Turn on Motor Clock Wise for Open
        //Light flash Green
        //Turn on Audible Speaker
      DigitalWrite(magLockClose, LOW);//disengages power to the lock
      DigitalWrite(motorCWSpin, HIGH);
      flashLight(doorStatusLightG);
      SpeakerAlert();

      //How to get to next state
        //if position is open detected
        //stop motor
        //power the open maglocks to keep the door in open position
        //go to door is open state
      if (DigitalRead(doorPositionLimitSwitchOpen) == HIGH){
        DigitalWrite(motorCWSpin, LOW);
        DigitalWrite(magLockOpen, HIGH);
        nextState(DOOR_IS_OPEN);
      }
      else if(DigitalRead(hindranceObstacleDetection) == HIGH){
        nextState(OBSTACLE_DETECTED);
      }
      break;

    case DOOR_IS_OPEN:
      //Do in this state
       //Make door status light solid Green;
      DigitalWrite(doorStatusLightG, HIGH);

      //How to get to next state
        // Wait for closing condition or emergency
      if (DigitalRead(emergencyReleaseButton) == HIGH){
        nextState(EMERGENCY_CLOSE);
      }
      else if ((DigitalRead(doorProximitySensor) == LOW) && (trainIsApproaching == False)){
        nextState(DOOR_CLOSING);
      }
      
      break;

    case DOOR_CLOSING:
      //Do in this state
        //close door by activating motor
        //unlock maglock
      DigitalWrite(magLockOpen, LOW);//disengages power to unlock the lock
      DigitalWrite(motorACWSpin, HIGH);
      flashLight(doorStatusLightR);
      SpeakerAlert();

      //How to get to next state
        //if limitswitch closed is detected then stop motor
        //activate maglock
      if (DigitalRead(doorPositionLimitSwitchClose) == HIGH){
        DigitalWrite(motorACWSpin, LOW);
        DigitalWrite(magLockClose, HIGH);
        nextState(IDLE);
      }
      else if(DigitalRead(hindranceObstacleDetection) == HIGH){
        nextState(OBSTACLE_DETECTED);
      }
      break;

    case OBSTACLE_DETECTED:
      //Do in this state
      

      //How to get to next state
      
      break;

    case EMERGENCY_OPEN:
      //Do in this state
      

      //How to get to next state
      
      break;
    
    case EMERGENCY_CLOSE:
      //Do in this state
      

      //How to get to next state
      
      break;
  }
}

void flashLight(int pin) {
  // Toggle the light for flashing
  static unsigned long lastFlashTime = 0;
  if (millis() - lastFlashTime >= 500) {
    lastFlashTime = millis();
    digitalWrite(pin, !digitalRead(pin)); // Toggle light state
  }
}

void SpeakerAlert()) {
  // Toggle the speaker to high
  // need more complex logic than simple high command such as specific frequency to omit
  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 500) {
    lastTime = millis();
    digitalWrite(audibleSpeaker, !digitalRead(audibleSpeaker)); // Toggle speaker for half second increments
  }
}


void setup() {
  Serial.begin(9600);

  // Initialize the inbuilt LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Input Pins 
  pinMode(doorProximitySensor, INPUT);
  pinMode(doorPositionLimitSwitchOpen, INPUT);
  pinMode(doorPositionLimitSwitchClose, INPUT);
  pinMode(hindranceObstacleDetection, INPUT);
  pinMode(emergencyStopButton, INPUT);

  // Output Pin Definitions
  pinMode(motorCWSpin, OUTPUT);
  pinMode(motorACWSpin, OUTPUT);
  pinMode(magLockClose, OUTPUT);
  pinMode(magLockOpen, OUTPUT);
  pinMode(audibleSpeaker, OUTPUT);
  pinMode(doorStatusLightR, OUTPUT);
  pinMode(doorStatusLightG, OUTPUT);
  pinMode(doorStatusLightY, OUTPUT);
  pinMode(systemStatusLED, OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

}

