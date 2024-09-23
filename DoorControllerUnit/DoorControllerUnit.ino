// Input Pin Definitions
#define trainIsApproaching 7 //input to alert train is approaching
#define doorProximitySensor A0 //detect train door in position of PSD // Possibly too hard need think of a better way logically
#define doorPositionLimitSwitchOpen A1 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define doorPositionLimitSwitchClose A2 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define hindranceObstacleDetection A3 //Infared or laser or scales to detect obstacle in oath of door // still figuring out hindrance obstacle detection 
#define emergencyReleaseButton A4 // Emergency stop button if door malfunctions to release the locking mechanism

//pin for fire mode, if off turn everything off flash LEDs

//possible pins for talking to raspberry pi for interface

// Output Pin Definitions
#define motorCWSpin 2 //DC or AC motor to control door movement // 2 Pins to switch the polarity // Clockwise
#define motorCCWSpin 3 //DC or AC motor to control door movement // 2 Pins to switch the polarity //Anticlockwise
#define magLockClose 4 //locking device of door // 2 maglocks one for open one for closed
#define magLockOpen 5 //locking device of door // 2 maglocks one for open one for closed
#define audibleSpeaker 9 //alerts of door status
#define doorStatusLightR 10 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Red
#define doorStatusLightY 11 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Yellow Amber
#define doorStatusLightG 12 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Green
#define systemStatusLED 13 //flashing LED to indicate system is working and running, if not flashing that indication that something is wrong

//add global variables here
//bool doorOpen = false;
//bool doorClosed = true;
bool doorOpeningflag = false;
bool doorClosingflag = false;
//bool obstacleDetectedflag = false;
bool trainIsApproachingflag = false; //this will be changed in the PCU interface, eg. signal from Metro
bool fireModeActiveflag = false;
unsigned long doorOpenTime = 0;
const long openDuration = 10000;  // 10 seconds
int remainingTime = 1;

enum States {
  IDLE,
  TRAIN_APPROACHING,
  DOOR_OPENING,
  DOOR_IS_OPEN,
  DOOR_CLOSING,
  OBSTACLE_DETECTED,
  EMERGENCY_OPEN,
  EMERGENCY_CLOSE,
  FIRE_MODE
};

States currentState = IDLE;

//Function Prototypes
void NextState(States newState);
void StateMachine();
void FlashLight(int pin);
void SpeakerAlert();
bool SafetyInterlock();
bool ObstacleCleared();

void setup() {
  Serial.begin(9600);

  // Input Pins
  pinMode(trainIsApproaching, INPUT_PULLUP); //7
  pinMode(doorProximitySensor, INPUT_PULLUP); //A0
  pinMode(doorPositionLimitSwitchOpen, INPUT_PULLUP); //A1
  pinMode(doorPositionLimitSwitchClose, INPUT_PULLUP); //A2
  pinMode(hindranceObstacleDetection, INPUT_PULLUP); //A3
  pinMode(emergencyReleaseButton, INPUT_PULLUP); //A4

  // Output Pin Definitions //2-5, 9-13
  pinMode(motorCWSpin, OUTPUT);
  pinMode(motorCCWSpin, OUTPUT);
  pinMode(magLockClose, OUTPUT);
  pinMode(magLockOpen, OUTPUT);

  pinMode(audibleSpeaker, OUTPUT);
  pinMode(doorStatusLightR, OUTPUT);
  pinMode(doorStatusLightY, OUTPUT);
  pinMode(doorStatusLightG, OUTPUT);
  pinMode(systemStatusLED, OUTPUT);

  //Output Pins Initial values
  digitalWrite(motorCWSpin, LOW);
  digitalWrite(motorCCWSpin, LOW);
  digitalWrite(magLockClose, HIGH);
  digitalWrite(magLockOpen, LOW);
  digitalWrite(audibleSpeaker, LOW);
  digitalWrite(doorStatusLightR, HIGH);
  digitalWrite(doorStatusLightY, LOW);
  digitalWrite(doorStatusLightG, LOW);
  digitalWrite(systemStatusLED, LOW);

}


void loop() {
  // put your main code here, to run repeatedly:
  StateMachine();

  if (fireModeActiveflag == true){
    NextState(FIRE_MODE);
  }

  if (digitalRead(trainIsApproaching) == HIGH){
    digitalWrite(systemStatusLED, HIGH);
    trainIsApproachingflag = true;
  }
  else {
    digitalWrite(systemStatusLED, LOW);
    trainIsApproachingflag = false;
  }
  
}

void NextState(States newState) {
  currentState = newState;
}

void FlashLight(int pin) {
  // Toggle the light for flashing
  static unsigned long lastFlashTime = 0;
  if (millis() - lastFlashTime >= 500) {
    lastFlashTime = millis();
    digitalWrite(pin, !digitalRead(pin)); // Toggle light state
  } 
}

void SpeakerAlert() {
  // Toggle the speaker to high
  // need more complex logic than simple high command such as specific frequency to omit
  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 1000) {
    lastTime = millis();
    digitalWrite(audibleSpeaker, !digitalRead(audibleSpeaker)); // Toggle speaker for half second increments
  }
}

bool SafetyInterlock() {
  //include all safety checks here before door operation can commence

  // Check if there's an obstacle detected
  if (digitalRead(hindranceObstacleDetection) == HIGH) {
    Serial.println("Safety interlock: Obstacle detected, door cannot operate.");
    return false;  // Obstacle detected, cannot proceed
  }

  // Check if the door position limit switches are not faulty
  if (digitalRead(doorPositionLimitSwitchOpen) == HIGH && digitalRead(doorPositionLimitSwitchClose) == HIGH) {
    Serial.println("Safety interlock: Something is wrong with the door position sensors");
    return false; 
  }

  if (digitalRead(emergencyReleaseButton) == HIGH) {
  Serial.println("Safety interlock: Emergency release activated.");
  return false;  // Emergency release prevents door operation
  }

  // If all checks pass, return true
  Serial.println("Safety interlock: All checks passed, door can operate.");
  return true;
}

bool ObstacleCleared(){
  //check obstacle is removed somehow
  if (digitalRead(hindranceObstacleDetection) == LOW){
    return true;
  }
}

void StateMachine() {
  switch (currentState)
  {
    case IDLE:
      //Do in this state
        //wait for train to approach with doors shut
        //doorClosed = true;
        //doorOpen = false;
        doorClosingflag = false;
        digitalWrite(doorStatusLightR, HIGH);

      //How to get to next state
        //if alert from system detects train is approaching change to train approaching state
        //if emergency release button is detected, move to emergency open state
      if (trainIsApproachingflag == true){
        NextState(TRAIN_APPROACHING);
        Serial.println(millis());
        Serial.println("Train Approaching");
        digitalWrite(doorStatusLightR, LOW);
      }
      else if (digitalRead(emergencyReleaseButton) == HIGH){
        NextState(EMERGENCY_OPEN);
        Serial.println(millis());
        Serial.println("Emergency Open");
        digitalWrite(doorStatusLightR, LOW);
      }
      break;

    case TRAIN_APPROACHING:
      //Do in this state
        //Flash Light Amber
      FlashLight(doorStatusLightY);

      //How to get to next state
        //if proximity sensor is high for train in position
        //and safety interlock determines everything is all safe
        //trigger Door Opening State
      if (digitalRead(emergencyReleaseButton) == HIGH){
        NextState(EMERGENCY_OPEN);
        Serial.println(millis());
        Serial.println("Emergency Open 2");
        digitalWrite(doorStatusLightY, LOW);
      }
      else if ((digitalRead(doorProximitySensor) == HIGH) && (SafetyInterlock() == true)){
        //need to add some delay to make sure the train is in the correct position
        digitalWrite(doorStatusLightY, LOW);
        NextState(DOOR_OPENING);
        Serial.println(millis());
        Serial.println("Door is Opening");
      }
      break;

    case DOOR_OPENING:
      //Do in this state
        //Unlock Maglock Close Pin
        //Turn on Motor Clock Wise for Open
        //Light flash Green
        //Turn on Audible Speaker
        //check for obstacles
      digitalWrite(magLockClose, LOW);//disengages power to the lock
      digitalWrite(motorCWSpin, HIGH);//open the doors
      FlashLight(doorStatusLightG);//signal Door is opening flashing green light
      SpeakerAlert();//Alert audibly that doors are opening
      doorOpeningflag = true;

      //How to get to next state
        //if position is open is detected
        //stop motor
        //power the open maglocks to keep the door in open position
        //go to door is open state
      if (digitalRead(doorPositionLimitSwitchOpen) == HIGH){
        digitalWrite(motorCWSpin, LOW); //power off the motor
        digitalWrite(magLockOpen, HIGH); //turn on the maglocks at the ends of the door to hold doors open
        NextState(DOOR_IS_OPEN);
        Serial.println(millis());
        Serial.println("Door is Open: waiting 10 seconds for passengers to board");
        digitalWrite(doorStatusLightG, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      else if(digitalRead(hindranceObstacleDetection) == HIGH){
        NextState(OBSTACLE_DETECTED);
        Serial.println(millis());
        Serial.println("Obstacle Detected");
        digitalWrite(doorStatusLightG, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      break;

    case DOOR_IS_OPEN:
      //Do in this state
       //Make door status light solid Green;
      digitalWrite(doorStatusLightG, HIGH);
      //doorOpen = true;
      //doorClosed = false;
      doorOpeningflag = false;

      //How to get to next state
        // Wait for closing condition or emergency
      if (doorOpenTime == 0) {
        doorOpenTime = millis();  // Record when the door opened
      }
      
      /*unsigned long elapsedTime = millis() - doorOpenTime;
      // elapsedTime/1000 = remainingTime
      if (elapsedTime == remainingTime*1000) {
        Serial.println(remainingTime);
        remainingTime = remainingTime + 1;
      }*/

      if (digitalRead(emergencyReleaseButton) == HIGH){
        NextState(EMERGENCY_CLOSE);
        Serial.println(millis());
        Serial.println("Emergency Close");
        digitalWrite(doorStatusLightG, LOW);
      }
      else if (millis() - doorOpenTime >= openDuration) {
        NextState(DOOR_CLOSING);  // Transition to closing state after 5 seconds
        doorOpenTime = 0;  // Reset for the next open cycle
        remainingTime = 0;
        Serial.println(millis());
        Serial.println("Door is Closing");
        digitalWrite(doorStatusLightG, LOW);
      }
      break;

    case DOOR_CLOSING:
      //Do in this state
        //close door by activating motor
        //unlock maglock
      digitalWrite(magLockOpen, LOW);//disengages power of end locks to unlock the lock so door can move
      digitalWrite(motorCCWSpin, HIGH); //power motor to close doors
      FlashLight(doorStatusLightR); //visualy alert door is closing with flashing red light
      SpeakerAlert(); //Audibly alert door is closing
      doorClosingflag = true;

      //How to get to next state
        //if limitswitch closed is detected then stop motor
        //activate maglock
      if (digitalRead(doorPositionLimitSwitchClose) == HIGH){
        digitalWrite(motorCCWSpin, LOW); //power off the motor
        digitalWrite(magLockClose, HIGH); //power maglocks to lock the doors
        NextState(IDLE);
        Serial.println(millis());
        Serial.println("Door is closed back to IDLE");
        digitalWrite(doorStatusLightR, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      else if(digitalRead(hindranceObstacleDetection) == HIGH){
        NextState(OBSTACLE_DETECTED);
        Serial.println(millis());
        Serial.println("Obstacle Detected 2");
        digitalWrite(doorStatusLightR, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      break;

    case OBSTACLE_DETECTED:
      //Do in this state
      digitalWrite(motorCWSpin, LOW); //power off the motor
      digitalWrite(motorCCWSpin, LOW); //power off the motor
      SpeakerAlert(); //Audibly alert Obstacle is in way
      FlashLight(doorStatusLightY); //flash amber light

      //How to get to next state
      if (ObstacleCleared()){
        digitalWrite(doorStatusLightY, LOW);
        digitalWrite(audibleSpeaker, LOW);
        if (doorOpeningflag == true){
          NextState(DOOR_OPENING);
          Serial.println(millis());
          Serial.println("Door is opening after Obstacle cleared");
        }
        else if (doorClosingflag == true){
          NextState(DOOR_CLOSING);
          Serial.println(millis());
          Serial.println("Door is closing after Obstacle cleared");
        }
      }
      
      break;

    case EMERGENCY_OPEN:
      //Do in this state
        //Unlock Maglock Close Pin
        //Turn on Motor Clock Wise for Open
        //Light flash Green
        //Turn on Audible Speaker
        //check for obstacles
      digitalWrite(magLockClose, LOW);//disengages power to the lock
      digitalWrite(motorCWSpin, HIGH);//open the doors
      FlashLight(doorStatusLightG);//signal Door is opening flashing green light
      SpeakerAlert();//Alert audibly that doors are opening
      doorOpeningflag = true;

      //How to get to next state
        //if position is open is detected
        //stop motor
        //power the open maglocks to keep the door in open position
        //go to door is open state
      if (digitalRead(doorPositionLimitSwitchOpen) == HIGH){
        digitalWrite(motorCWSpin, LOW); //power off the motor
        digitalWrite(magLockOpen, HIGH); //turn on the maglocks at the ends of the door to hold doors open
        NextState(DOOR_IS_OPEN);
        Serial.println(millis());
        Serial.println("Door is open after emergency release pressed");
        digitalWrite(doorStatusLightG, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      /*else if(digitalRead(hindranceObstacleDetection) == HIGH){
        NextState(OBSTACLE_DETECTED);
        Serial.println(millis());
        Serial.println("Obstacle ");
      }*/
      break;
    
    case EMERGENCY_CLOSE:
      //Do in this state
        //close door by activating motor
        //unlock maglock
      digitalWrite(magLockOpen, LOW);//disengages power of end locks to unlock the lock so door can move
      digitalWrite(motorCCWSpin, HIGH); //power motor to close doors
      FlashLight(doorStatusLightR); //visualy alert door is closing with flashing red light
      SpeakerAlert(); //Audibly alert door is closing
      doorClosingflag = true;

      //How to get to next state
        //if limitswitch closed is detected then stop motor
        //activate maglock
      if (digitalRead(doorPositionLimitSwitchClose) == HIGH){
        digitalWrite(motorCCWSpin, LOW); //power off the motor
        digitalWrite(magLockClose, HIGH); //power maglocks to lock the doors
        NextState(IDLE);
        Serial.println(millis());
        Serial.println("Door is closing after Emergency Release pressed");
        digitalWrite(doorStatusLightR, LOW);
        digitalWrite(audibleSpeaker, LOW);
      }
      /*else if(digitalRead(hindranceObstacleDetection) == HIGH){
        NextState(OBSTACLE_DETECTED);
      }*/
      break;

    case FIRE_MODE:
      //Do in this state
        // Stop all operations in case of emergency
      digitalWrite(motorCWSpin, LOW);
      digitalWrite(motorCCWSpin, LOW);
      digitalWrite(magLockOpen, LOW);
      digitalWrite(magLockClose, LOW);
      SpeakerAlert();
      FlashLight(doorStatusLightR);

      //How to get to next state
      break;
  }
}

/* DOOR POSITION FOR EACH STATE
Door Fully Open:
doorPositionLimitSwitchOpen = HIGH A1
doorPositionLimitSwitchClose = LOW A2

Door Fully Closed:
doorPositionLimitSwitchOpen = LOW
doorPositionLimitSwitchClose = HIGH

Door In Motion:
doorPositionLimitSwitchOpen = LOW
doorPositionLimitSwitchClose = LOW
*/

