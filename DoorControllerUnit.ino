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

//functions to control the input devices

void DoorProximitySensorOperation() {

}

void DoorPositionDetectionOperation() {

}

void HindranceObstacleDetectionOperation() {

}

void doorPushButtonOperation() {

}

void EmergencyReleaseLevelOperation() {

}

void EmergencyStopButtonOperation() {

}

//functions to control the output devices
void motorOperationOperation() {

}
void MagLockOperation() {

}

void AudibleSpeakerOperation() {

}
void DoorStatusLight() {

}

//Logic input/output functions
void DoorPredictionSystem() {
  //variable receiving data from the overall train control system, indicating when a train is approaching.
}

void SafetyInterlock() {
  //ensures all conditions are safe for the doors to operate 
}

//Logic Operations

void TrainApproaching() {

}

void DoorOpeningSequence() {

}

void DoorClosingSequence() {

}

void EmergencySituations() {

}

void DoorOperation() {

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

  
}

