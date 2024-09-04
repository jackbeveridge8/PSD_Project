// Define the inbuilt Arduino LED pin
#define ledPin 13

// Input Pin Definitions
#define doorProximitySensor 2 //detect train door in position of PSD // Possibly too hard need think of a better way logically
#define doorPositionLimitSwitchOpen 3 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define doorPositionLimitSwitchClose 4 //Detects Door open, close or opening // might be logic function instead of physical device // limit switch 1 for open, 1 for closed. // work hand in hand with motor
#define hindranceObstacleDetection 5 //Infared or laser or scales to detect obstacle in oath of door // still figuring out hindrance obstacle detection 
#define emergencyStopButton 6 // Emergency stop button if door malfunctions to release the locking mechanism

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


//add global variables here
bool doorOpen = false;
bool doorLocked = true;
bool obstacleDetected = false;


//------------------------------------------------------------
//***********functions to control the INPUT devices***********
void DoorProximitySensorOperation() {
//Function to collect and prepare the data if needed

}

void DoorPositionDetectionOperation() {
  // Collect data from the door position limit switches
  if (digitalRead(doorPositionLimitSwitchOpen) == HIGH) {
    doorOpen = true;
  }
  if (digitalRead(doorPositionLimitSwitchClose) == HIGH) {
    doorOpen = false;
  }
}

void HindranceObstacleDetectionOperation() {
  // Detect obstacles in the door's path
  if (digitalRead(hindranceObstacleDetection) == HIGH) {
    obstacleDetected = true;
  } else {
    obstacleDetected = false;
  }
}

void doorPushButtonOperation() {
//Function to collect and prepare the data if needed
}

void EmergencyReleaseLevelOperation() {
  if (digitalRead(emergencyStopButton) == HIGH) 
  {
    MagLockOperation(false);  // Unlock the door
    motorOperation(false);    // Stop the motor
    AudibleSpeakerOperation(true);  // Activate the alarm
  }
}

//-------------------------------------------------------------
//***********functions to control the OUTPUT devices***********
void motorOperationOperation() {
//Function to operate the motor and send the correct signal if needed
}
void MagLockOperation() {
//Function to operate the mag lock and send the correct signal if needed

//When it gets powered, magnet turns on
//When power turns off, magnet turns off
}

void AudibleSpeakerOperation() {
//Function to operate the speaker and send the correct signal if needed
}

string DoorStatusLight(String Colour) {
//Function to operate the LED Light on door and send the correct signal if needed
  if (Colour == "green")
  {
    digitalWrite(doorStatusLight, HIGH);
  }
}

//possible statemachine process to show which pins on and off 

//--------------------------------------------------
//***********Logic input/output functions***********
void DoorPredictionSystem() {
  //variable receiving data from the overall train control system, indicating when a train is approaching.
  //Signals when the train is approaching the platform.
}

void SafetyInterlock() {
  //ensures all conditions are safe for the doors to operate 
}

//-------------------------------------------------------
////***********LOGIC PSD OPERATION FUNCTIONS//***********

void TrainApproaching() {
  //When the Door Prediction system signals that a train is approaching
    // if DoorPredictionSystem = True
  //And the Door Proximity Sensor detects the train is close enough
    // if 
			//Activate the Visual Lights colour green to signal that the doors are about to open.
      DoorStatusLight("green") //flashing
	//If the Safety Interlock Function is confirmed 
  //And no obstacles are detected by the Hindrance Obstacle Detector
      //Unlock the doors by deactivating the Door Status Magnetic Lock.
}

//make method fucntion for unlocking door maglock

void DoorOpeningSequence() { //you can set to close before its close it will lock when close enough 
  //If the doors are unlocked 
			//Activate the Motor Control to open the doors.
   		//Activate Visual light to green colour flashing for opening
	  	//Activate Audible Bell for door opening
  //Monitor the Door Position Detection to track the door's status during the opening.
			//Output to console the status
  //If any obstacle is detected by the Hindrance Obstacle Detector
	    //Stop the motor 
   		//Trigger the Audible Bell Speaker to alert.
	  	//Activate Visual light to warning colour (orange?)
	 		//Continue closing after the obstacle removed
  //If the door fully opens without hindrance
			//Signal the system to maintain the open position.
   		//Visual light colour green steady on
}

void DoorClosingSequence() { //you can set to close before its close it will lock when close enough 
  //Once the Safety Interlock releases
  //And no obstacles are detected
      //Activate the Motor Control to close the doors.
      //Activate visual light to red flashing.
      //Activate audible bell for door closing
  //Continuously monitor the Door Position Detection during closure.
      //Output to console the status
  //If an obstacle is detected during the closing
      //Stop the motor
      //Trigger the Audible Bell Speaker to alert.
      //Activate Visual light to warning colour (orange?)
      //Keep the doors open until the obstacle is cleared and continue closing
  //Once fully closed
      //activate the Door Magnetic Lock to secure the doors.
}

void EmergencySituations() {
  //If the Emergency Release Mechanism is triggered
			//Immediately unlock the doors releasing the maglock
   		//Open them using the Motor Control or manually pull them open
	  	//Activate Audible Bell
	 		//Activate Visual red flashing light
  //If The Emergency Close Stop Button is triggered
			//Halt any door movement and keep the door in its current position when pressed.
   		//Stop Motor
	  	//Activate Audible Bell
	 		//Activate Visual red flashing light
}

void TotalDoorOperation() {
  //function to controll the other functions and be called in the main loop?
}

void setup() {
  Serial.begin(9600); // Start serial communication for debugging

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  // Input Pins 
  pinMode(doorProximitySensor, INPUT);
  pinMode(doorPositionDetection, INPUT);
  pinMode(hindranceObstacleDetection, INPUT);
  pinMode(doorPushButton, INPUT);
  pinMode(emergencyReleaseLever, INPUT);
  pinMode(emergencyStopButton, INPUT);
  pinMode(safetyInterlock, INPUT);
  // Output Pin Definitions
  pinMode(motor, OUTPUT);
  pinMode(magLock, OUTPUT);
  pinMode(audibleSpeaker, OUTPUT);
  pinMode(doorStatusLight, OUTPUT);
}

// Main loop to control and read the pins
void loop() {
  // Turn the LED on (HIGH is the voltage level)
  //digitalWrite(ledPin, HIGH);
  // Wait for a second
  //delay(1000);
  // Turn the LED off by making the voltage LOW
  //digitalWrite(ledPin, LOW);
  // Wait for a second
  //delay(1000);

  digitalWrite(motor, HIGH);
  digitalWrite(magLock, HIGH);
  digitalWrite(audibleSpeaker, HIGH);
  digitalWrite(doorStatusLight, HIGH);
  delay(1000);
  digitalWrite(motor, LOW);
  digitalWrite(magLock, LOW);
  digitalWrite(audibleSpeaker, LOW);
  digitalWrite(doorStatusLight, LOW);
  delay(1000);

  //TotalDoorOperation();

  
}

