#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define HENRY

#ifdef JACK
    const char* ssid = "TelstraA4B191";  // Replace with your WiFi SSID
    const char* password = "3dsxbhme2h";  // Replace with your WiFi password

    #define debugLED 13 // Specify the GPIO pin number for the LED

    // Input Pin Definitions
    #define doorPositionLimitSwitchOpen 27 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
    #define doorPositionLimitSwitchClose 26 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
    #define hindranceObstacleDetection 25 //Infared or laser or scales to detect obstacle in oath of door // still figuring out hindrance obstacle detection 
    #define emergencyReleaseButton 33 // Emergency stop button if door malfunctions to release the locking mechanism

    // Output Pin Definitions
    #define motorCWSpin 15 //DC or AC motor to control door movement // 2 Pins to switch the polarity // Clockwise
    #define motorCCWSpin 4 //DC or AC motor to control door movement // 2 Pins to switch the polarity //Anticlockwise
    #define magLockClose 5 //locking device of door // 2 maglocks one for open one for closed
    #define audibleSpeaker 19 //alerts of door status
    #define doorStatusLightR 21 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Red
    #define doorStatusLightB 22 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Yellow Amber
    #define doorStatusLightG 23 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Green
    #define systemStatusLED 2 //flashing LED to indicate system is working and running, if not flashing that indication that something is wrong
#endif

#ifdef HENRY
    const char* ssid = "Endora Fan Club";  // Replace with your WiFi SSID
    const char* password = "stinkygirl";  // Replace with your WiFi password

    #define debugLED 13 // Specify the GPIO pin number for the LED

    // Input Pin Definitions
    #define doorPositionLimitSwitchOpen 27 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
    #define doorPositionLimitSwitchClose 26 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
    #define hindranceObstacleDetection 34 //Infared or laser or scales to detect obstacle in oath of door // still figuring out hindrance obstacle detection 
    #define emergencyReleaseButton 23 // Emergency stop button if door malfunctions to release the locking mechanism

    // Output Pin Definitions
    #define motorCWSpin 25 //DC or AC motor to control door movement // 2 Pins to switch the polarity // Clockwise
    #define motorCCWSpin 33 //DC or AC motor to control door movement // 2 Pins to switch the polarity //Anticlockwise
    #define magLockClose 32 //locking device of door // 2 maglocks one for open one for closed
    #define audibleSpeaker 19 //alerts of door status
    #define doorStatusLightR 21 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Red
    #define doorStatusLightB 22 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Yellow Amber
    #define doorStatusLightG 23 //visual alert of door status // 1 LED with 3 pins to control 3 different lights //Green
    #define systemStatusLED 2 //flashing LED to indicate system is working and running, if not flashing that indication that something is wrong
#endif

AsyncWebServer server(80);

//add global variables here
bool doorOpeningflag = false;
bool doorClosingflag = false;
bool trainIsApproachingFlag = false; //this will be changed in the PCU interface, eg. signal from Metro
bool doorProximitySensorFlag = false;
bool hindranceObstacleDetectionFlag = false;
bool emergencyReleaseButtonFlag = false;
bool fireModeActiveflag = false;
unsigned long doorOpenTime = 0;
const long openDuration = 10000;  // 10 seconds
int remainingTime = 1;

bool speakerOn = false;
static unsigned long lastFlashTime = 0;
static unsigned long lastSpeakerTime = 0;
static unsigned long flashRate = 500;
static unsigned long speakerRate = 500;



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
void FlashLight(int colour);
void TurnOnLight(int colour);
void SpeakerAlert();
bool SafetyInterlock();
bool ObstacleCleared();
void wifiSetup();
void setInputValue(int pin, String value);

void setup() {
  Serial.begin(115200);

  // Input Pins
  pinMode(doorPositionLimitSwitchOpen, INPUT_PULLUP);
  pinMode(doorPositionLimitSwitchClose, INPUT_PULLUP);
  pinMode(hindranceObstacleDetection, INPUT);
  pinMode(emergencyReleaseButton, INPUT);

  // Output Pin Definitions
  pinMode(motorCWSpin, OUTPUT);
  pinMode(motorCCWSpin, OUTPUT);
  pinMode(magLockClose, OUTPUT);
  ledcAttach(audibleSpeaker, 1000, 8);
  pinMode(doorStatusLightR, OUTPUT);
  pinMode(doorStatusLightB, OUTPUT);
  pinMode(doorStatusLightG, OUTPUT);
  pinMode(systemStatusLED, OUTPUT);

  //debug LED pin
  pinMode(debugLED, OUTPUT);
  digitalWrite(debugLED, LOW);  // Start with LED off

  //Output Pins Initial values
  digitalWrite(motorCWSpin, LOW);
  digitalWrite(motorCCWSpin, LOW);
  digitalWrite(magLockClose, HIGH);
  ledcWrite(audibleSpeaker, 0);
  digitalWrite(doorStatusLightR, LOW);
  digitalWrite(doorStatusLightB, LOW);
  digitalWrite(doorStatusLightG, LOW);
  digitalWrite(systemStatusLED, LOW);

  wifiSetup();

}


void loop() {
  // put your main code here, to run repeatedly:
  StateMachine();

  if (fireModeActiveflag){
    NextState(FIRE_MODE);
  }
  
}

void wifiSetup(){
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure server routes
  server.on("/setInput", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("pin") && request->hasParam("value")) {
      int pin = request->getParam("pin")->value().toInt();
      String value = request->getParam("value")->value();
      setInputValue(pin, value);
      request->send(200, "text/plain", "Input set");
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  server.on("/getInputStates", HTTP_GET, [](AsyncWebServerRequest *request){
    String jsonResponse = getInputStatesJSON();
    request->send(200, "application/json", jsonResponse);
  });


  server.on("/getState", HTTP_GET, [](AsyncWebServerRequest *request){
    String stateStr = getStateString();
    request->send(200, "text/plain", stateStr);
  });

  server.on("/toggleDebugLED", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();
      if (state == "ON") {
        digitalWrite(debugLED, HIGH);
      } else if (state == "OFF") {
        digitalWrite(debugLED, LOW);
      }
      request->send(200, "text/plain", "Debug LED state changed");
      Serial.println(state);
    } else {
      request->send(400, "text/plain", "Bad Request: 'state' parameter missing");
    }
  });

  server.on("/getDebugLEDState", HTTP_GET, [](AsyncWebServerRequest *request){
  bool ledState = digitalRead(debugLED);
  String response = ledState ? "ON" : "OFF";
  request->send(200, "text/plain", response);
  });


  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void setInputValue(int pin, String value) {
  bool val;
  if (value == "HIGH") {
      val = true;
  } else {
      val = false;
  }

  switch (pin) {
    case 12:
      trainIsApproachingFlag = val;
      break;
    case 14:
      doorProximitySensorFlag = val;
      break;
    case 25:
      hindranceObstacleDetectionFlag = val;
      break;
    case 33:
      emergencyReleaseButtonFlag = val;
      break;
    case 1:
      fireModeActiveflag = val;
      break;
    default:
      Serial.println("Invalid pin");
      break;
  }
  Serial.printf("Input %d set to %s\n", pin, value.c_str());
}

String getStateString() {
  switch (currentState) {
    case IDLE:
      return "IDLE";
    case TRAIN_APPROACHING:
      return "TRAIN_APPROACHING";
    case DOOR_OPENING:
      return "DOOR_OPENING";
    case DOOR_IS_OPEN:
      return "DOOR_IS_OPEN";
    case DOOR_CLOSING:
      return "DOOR_CLOSING";
    case OBSTACLE_DETECTED:
      return "OBSTACLE_DETECTED";
    case EMERGENCY_OPEN:
      return "EMERGENCY_OPEN";
    case EMERGENCY_CLOSE:
      return "EMERGENCY_CLOSE";
    case FIRE_MODE:
      return "FIRE_MODE";
    default:
      return "UNKNOWN";
  }
}

String getInputStatesJSON() {
    // Create a JSON-formatted string
    String json = "{";
    json += "\"trainIsApproaching\": {\"software\":" + String(trainIsApproachingFlag ? "true" : "false") + "},";
    json += "\"doorProximitySensor\": {\"software\":" + String(doorProximitySensorFlag ? "true" : "false") + "},";
    json += "\"hindranceObstacleDetection\": {\"hardware\":" + String(digitalRead(hindranceObstacleDetection) == HIGH ? "true" : "false") +
            ", \"software\":" + String(hindranceObstacleDetectionFlag ? "true" : "false") + "},";
    json += "\"emergencyReleaseButton\": {\"hardware\":" + String(digitalRead(emergencyReleaseButton) == HIGH ? "true" : "false") +
            ", \"software\":" + String(emergencyReleaseButtonFlag ? "true" : "false") + "}";
    json += "}";

    return json;
}



void NextState(States newState) {
  currentState = newState;
}

void FlashLight(int colour) {
  // Toggle the light for flashing

  if (millis() - lastFlashTime >= flashRate) {
    lastFlashTime = millis();
    if (colour == 1) {
      digitalWrite(doorStatusLightR, !digitalRead(doorStatusLightR));
      digitalWrite(doorStatusLightG, LOW);
      digitalWrite(doorStatusLightB, LOW);
    }
    else if (colour == 2) {
      digitalWrite(doorStatusLightR, LOW);
      digitalWrite(doorStatusLightG, !digitalRead(doorStatusLightG));
      digitalWrite(doorStatusLightB, LOW);

    }
    else if (colour == 3) {
      digitalWrite(doorStatusLightR, LOW);
      digitalWrite(doorStatusLightG, LOW);
      digitalWrite(doorStatusLightB, !digitalRead(doorStatusLightB));
    }
    else {
      digitalWrite(doorStatusLightR, LOW);
      digitalWrite(doorStatusLightG, LOW);
      digitalWrite(doorStatusLightB, LOW);
    }
  } 
}

void TurnOnLight(int colour) {
  if (colour == 1) {
    digitalWrite(doorStatusLightR, HIGH);
    digitalWrite(doorStatusLightG, LOW);
    digitalWrite(doorStatusLightB, LOW);
  }
  else if (colour == 2) {
    digitalWrite(doorStatusLightR, LOW);
    digitalWrite(doorStatusLightG, HIGH);
    digitalWrite(doorStatusLightB, LOW);

  }
  else if (colour == 3) {
    digitalWrite(doorStatusLightR, LOW);
    digitalWrite(doorStatusLightG, LOW);
    digitalWrite(doorStatusLightB, HIGH);
  }
  else {
    digitalWrite(doorStatusLightR, LOW);
    digitalWrite(doorStatusLightG, LOW);
    digitalWrite(doorStatusLightB, LOW);
  }
}

void SpeakerAlert() {
  // Toggle the speaker to high
  // need more complex logic than simple high command such as specific frequency to omit
  if (millis() - lastSpeakerTime >= speakerRate) {
    lastSpeakerTime = millis();
    if (lastSpeakerTime == false) {
      ledcWrite(audibleSpeaker, 128);
      speakerOn = true;
    } else {
      ledcWrite(audibleSpeaker, 0);
      speakerOn = false;
    }
    
  }
}

bool SafetyInterlock() {
  //include all safety checks here before door operation can commence

  // Check if there's an obstacle detected
  if (digitalRead(hindranceObstacleDetection) == HIGH || hindranceObstacleDetectionFlag == true) {
    Serial.println("Safety interlock: Obstacle detected, door cannot operate.");
    return false;  // Obstacle detected, cannot proceed
  }

  // Check if the door position limit switches are not faulty
  if (digitalRead(doorPositionLimitSwitchOpen) == HIGH && digitalRead(doorPositionLimitSwitchClose) == HIGH) {
    Serial.println("Safety interlock: Something is wrong with the door position sensors");
    return false; 
  }

  if (digitalRead(emergencyReleaseButton) == HIGH || emergencyReleaseButtonFlag == true) {
  Serial.println("Safety interlock: Emergency release activated.");
  return false;  // Emergency release prevents door operation
  }

  // If all checks pass, return true
  Serial.println("Safety interlock: All checks passed, door can operate.");
  return true;
}

bool ObstacleCleared(){
  //check obstacle is removed somehow
  if (digitalRead(hindranceObstacleDetection) == LOW || hindranceObstacleDetectionFlag == false){
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
        TurnOnLight(1);

      //How to get to next state
        //if alert from system detects train is approaching change to train approaching state
        //if emergency release button is detected, move to emergency open state
      if (trainIsApproachingFlag == true){
        NextState(TRAIN_APPROACHING);
        Serial.println(millis());
        Serial.println("Train Approaching");
        TurnOnLight(0);
      }
      else if (digitalRead(emergencyReleaseButton) == HIGH || emergencyReleaseButtonFlag == true){
        NextState(EMERGENCY_OPEN);
        Serial.println(millis());
        Serial.println("Emergency Open");
        TurnOnLight(0);
      }
      break;

    case TRAIN_APPROACHING:
      //Do in this state
        //Flash Light Amber
      FlashLight(3);

      //How to get to next state
        //if proximity sensor is high for train in position
        //and safety interlock determines everything is all safe
        //trigger Door Opening State
      if (digitalRead(emergencyReleaseButton) == HIGH || emergencyReleaseButtonFlag == true){
        NextState(EMERGENCY_OPEN);
        Serial.println(millis());
        Serial.println("Emergency Open 2");
        TurnOnLight(0);
      }
      else if ((doorProximitySensorFlag == true) && (SafetyInterlock() == true)){
        //need to add some delay to make sure the train is in the correct position
        TurnOnLight(0);
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
      FlashLight(2);//signal Door is opening flashing green light
      SpeakerAlert();//Alert audibly that doors are opening
      doorOpeningflag = true;

      //How to get to next state
        //if position is open is detected
        //stop motor
        //power the open maglocks to keep the door in open position
        //go to door is open state
      if (digitalRead(doorPositionLimitSwitchOpen) == HIGH){
        digitalWrite(motorCWSpin, LOW); //power off the motor
        NextState(DOOR_IS_OPEN);
        Serial.println(millis());
        Serial.println("Door is Open: waiting 10 seconds for passengers to board");
        TurnOnLight(0);
        ledcWrite(audibleSpeaker, 0);
      }
      else if(digitalRead(hindranceObstacleDetection) == HIGH || hindranceObstacleDetectionFlag == true){
        NextState(OBSTACLE_DETECTED);
        Serial.println(millis());
        Serial.println("Obstacle Detected");
        TurnOnLight(0);
        ledcWrite(audibleSpeaker, 0);
      }
      break;

    case DOOR_IS_OPEN:
      //Do in this state
       //Make door status light solid Green;
      TurnOnLight(2);
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

      if (digitalRead(emergencyReleaseButton) == HIGH || emergencyReleaseButtonFlag == true){
        NextState(EMERGENCY_CLOSE);
        Serial.println(millis());
        Serial.println("Emergency Close");
        TurnOnLight(0);
      }
      else if (millis() - doorOpenTime >= openDuration) {
        NextState(DOOR_CLOSING);  // Transition to closing state after 5 seconds
        doorOpenTime = 0;  // Reset for the next open cycle
        remainingTime = 0;
        Serial.println(millis());
        Serial.println("Door is Closing");
        TurnOnLight(0);
      }
      break;

    case DOOR_CLOSING:
      //Do in this state
        //close door by activating motor
        //unlock maglock
      digitalWrite(motorCCWSpin, HIGH); //power motor to close doors
      FlashLight(1); //visualy alert door is closing with flashing red light
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
        ledcWrite(audibleSpeaker, 0);
      }
      else if(digitalRead(hindranceObstacleDetection) == HIGH || hindranceObstacleDetectionFlag == true){
        NextState(OBSTACLE_DETECTED);
        Serial.println(millis());
        Serial.println("Obstacle Detected 2");
        TurnOnLight(0);
        ledcWrite(audibleSpeaker, 0);
      }
      break;

    case OBSTACLE_DETECTED:
      //Do in this state
      digitalWrite(motorCWSpin, LOW); //power off the motor
      digitalWrite(motorCCWSpin, LOW); //power off the motor
      SpeakerAlert(); //Audibly alert Obstacle is in way
      FlashLight(3); //flash amber light

      //How to get to next state
      if (ObstacleCleared()){
        digitalWrite(doorStatusLightB, LOW);
        ledcWrite(audibleSpeaker, 0);
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
      FlashLight(2);//signal Door is opening flashing green light
      SpeakerAlert();//Alert audibly that doors are opening
      doorOpeningflag = true;

      //How to get to next state
        //if position is open is detected
        //stop motor
        //power the open maglocks to keep the door in open position
        //go to door is open state
      if (digitalRead(doorPositionLimitSwitchOpen) == HIGH){
        digitalWrite(motorCWSpin, LOW); //power off the motor
        NextState(DOOR_IS_OPEN);
        Serial.println(millis());
        Serial.println("Door is open after emergency release pressed");
        digitalWrite(doorStatusLightG, LOW);
        ledcWrite(audibleSpeaker, 0);
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
      digitalWrite(motorCCWSpin, HIGH); //power motor to close doors
      FlashLight(1); //visualy alert door is closing with flashing red light
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
        ledcWrite(audibleSpeaker, 0);
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
      digitalWrite(magLockClose, LOW);
      SpeakerAlert();
      FlashLight(1);

      //How to get to next state
      if (!fireModeActiveflag){
        NextState(IDLE);
      }
      break;
  }
}
