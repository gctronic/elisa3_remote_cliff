# Elisa-3 remote cliff avoidance
This is a terminal application implementing cliff avoidance behavior controlling the robot from the computer through the radio base-station, this means that the robot reacts only to the commands received from the computer and has no "intelligence" onboard. The demo uses the information gathered from the 4 ground sensors to stop the robot when a cliff is detected (threshold tuned to run in a white surface); moreover the RGB LEDs are updated with a random color at fixed intervals. <br/>
The project depends on the [Elisa-3 remote library](http://www.gctronic.com/doc/index.php/Elisa-3#Elisa-3_library).<br/>
For more information refer to the wiki [Elisa-3 Cliff avoidance](https://www.gctronic.com/doc/index.php?title=Elisa-3#Cliff_avoidance).
