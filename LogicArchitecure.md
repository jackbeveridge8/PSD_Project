**.DCU Software Plan Architecture.**

**Components to control:**
	Door Proximity
	Door Prediction
	Door Position Detection (open/close/in-motion)
	Hindrance Obstacle Detetcor
	Door Status Mag Lock (Locked/Unlocked)
	Audible Bell Speaker
	Visual Lights
	Motor 
	Button Activation
	Emergency Release Mechanism
	Emergrency Close stop button
	Safety interlock

**Establish Input and Output Pins**
**Input Pins:**
	Door Proximity Sensor (Input)
        \\Detects if the platform is close to the train.
	Door Prediction System (Input)
		\\Signals when the train is approaching the platform.
	Door Position Detection (Input)
		\\Provides the current status of the door: open, closed, or in motion.
	Hindrance Obstacle Detector (Input)
		\\Detects obstacles in the door’s path.
	Button Activation (Input)
		\\Manually opens the doors.
	Emergency Release Mechanism (Input)
		\\Signals an emergency situation requiring the doors to be opened manually.
	Emergency Close Stop Button (Input)
		\\Immediately stops the door in case of an emergency.
	Safety Interlock (Input)
		\\Ensures all conditions are safe for the doors to operate.

**Output Pins:**
	Motor Control (Output)
		\\Controls the movement of the doors (open/close).
	Door Status Magnetic Lock (Output)
		\\Controls the locking mechanism to secure the doors.
	Audible Bell Speaker (Output)
		\\Emits a sound to indicate door movement or an alert.
	Visual Lights (Output)
		\\Indicates the door status (red for locked, green for unlocked).

**Logic:**
		\\if
  			\\do
	 
	Train Approaching (Prediction System & Proximity Sensor)
		\\When the Door Prediction system signals that a train is approaching
  		\\and the Door Proximity Sensor detects the train is close enough
			\\activate the Visual Lights to signal that the doors are about to open.
		\\If the Safety Interlock is confirmed 
  		\\and no obstacles are detected by the Hindrance Obstacle Detector
  			\\unlock the doors by deactivating the Door Status Magnetic Lock.
        
    Door Opening Sequence
        \\If the doors are unlocked
			\\activate the Motor Control to open the doors.
   			\\Activate Visual light to green colour flashing for opening
	  		\\Activate Audible Bell for door opening
        \\Monitor the Door Position Detection to track the door's status during the opening.
			\\output to console the status
        \\If any obstacle is detected by the Hindrance Obstacle Detector
			\\stop the motor 
   			\\trigger the Audible Bell Speaker to alert.
	  		\\Activate Visual light to warning colour (orange?)
	 		\\continue closing after obstacle removed
        \\If the door fully opens without hindrance
			\\signal the system to maintain the open position.
   			\\visual light colour green steady on
        
    Door Closing Sequence
        \\Once the Safety Interlock releases
		\\and no obstacles are detected
  			\\activate the Motor Control to close the doors.
	 		\\Activate visual light to red flashing
			\\Activate audible bell for door closing
        \\Continuously monitor the Door Position Detection during closure.
			\\output to console the status
        \\If an obstacle is detected during closing
			\\stop the motor 
   			\\trigger the Audible Bell Speaker to alert.
	  		\\Activate Visual light to warning colour (orange?)
   			\\keep the doors open until the obstacle is cleared and continue closing
        \\Once fully closed, activate the Door Status Magnetic Lock to secure the doors.

    Emergency Situations
        \\If the Emergency Release Mechanism is triggered
			\\immediately unlock the doors releasing maglock
   			\\open them using the Motor Control or manually pull open
	  		\\Activate Audible Bell
	 		\\Activate Visual red flashing light
        \\If The Emergency Close Stop Button is triggered
			\\halt any door movement and keep the door in its current position when pressed.
   			\\Stop Motor
	  		\\Activate Audible Bell
	 		\\Activate Visual red flashing light

    Manual Operation (Button Activation)
        \\When the Button Activation input is detected, follow the opening sequence if the doors are closed or the closing sequence if the doors are open.
