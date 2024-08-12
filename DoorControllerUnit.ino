// Define the inbuilt Arduino LED pin
#define ledPin 13
// Input Pin Definitions
#define doorProximitySensor 2 //detect train door in position of PSD
#define doorPositionDetection 3 //Detects Door open, close or opening // might be logic function instead of physical device
#define hindranceObstacleDetection 4 //infared or laser or scales to detect obstacle in oath of door
#define doorPushButton 5 //Push button on door
#define emergencyReleaseLever 6 //Emergency Access Device if door malfunctions
#define emergencyStopButton 7 //emergency stop button if door malfunctions
#define safetyInterlock 8 //ensures all conditions are safe for the doors to operate // might be logic function instead of physical device

// Output Pin Definitions
#define motor 9 //DC or AC motor to control door movement
#define magLock 10 //locking device of door
#define audibleSpeaker 11 //alerts of door status
#define doorStatusLight 12 //visual alert of door status

//functions to control the INPUT devices
void DoorProximitySensorOperation() {
//Function to collect and prepare the data if needed
}

void DoorPositionDetectionOperation() {
//Function to collect and prepare the data if needed
}

void HindranceObstacleDetectionOperation() {
//Function to collect and prepare the data if needed
}

void doorPushButtonOperation() {
//Function to collect and prepare the data if needed
}

void EmergencyReleaseLevelOperation() {
//Function to collect and prepare the data if needed
}

void EmergencyStopButtonOperation() {
//Function to collect and prepare the data if needed
}

//functions to control the OUTPUT devices
void motorOperationOperation() {
//Function to operate the motor and send the correct signal if needed
}
void MagLockOperation() {
//Function to operate the mag lock and send the correct signal if needed
}

void AudibleSpeakerOperation() {
//Function to operate the speaker and send the correct signal if needed
}

string DoorStatusLight(string Colour) {
//Function to operate the LED Light on door and send the correct signal if needed
}

//Logic input/output functions
void DoorPredictionSystem() {
  //variable receiving data from the overall train control system, indicating when a train is approaching.
}

void SafetyInterlock() {
  //ensures all conditions are safe for the doors to operate 
}


//LOGIC PSD OPERATION FUNCTIONS

void TrainApproaching() {
  //When the Door Prediction system signals that a train is approaching
  //And the Door Proximity Sensor detects the train is close enough
			//Activate the Visual Lights colour green to signal that the doors are about to open.
      DoorStatusLight("green") //flashing
	//If the Safety Interlock is confirmed 
      //And no obstacles are detected by the Hindrance Obstacle Detector
      //Unlock the doors by deactivating the Door Status Magnetic Lock.
}

void DoorOpeningSequence() {
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

void DoorClosingSequence() {

}

void EmergencySituations() {

}

void TotalDoorOperation() {

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
  digitalWrite(ledPin, HIGH);
  // Wait for a second
  delay(1000);
  // Turn the LED off by making the voltage LOW
  digitalWrite(ledPin, LOW);
  // Wait for a second
  delay(1000);

  TotalDoorOperation();

  
}

