# Mushroom_Orchid_growbox
A habitat automatically set to the right conditions of humidity and temperature of your favourit plant or fungi

This code is written to work on most Arduino devices, the PIN's are interchangeable you just need to define
them except for PIN 2 this one is defined by the Dimmer library, depending on which device you are using this might be different, for more 
information i suggest to look up the https://www.arduino.cc/reference/en/libraries/gyverdimmer/

The code in the ino file uses the Gyver Dimmer library and the PID library available through the Arduino IDE library platform, 
many thanks to these programmers for offering their library code.

The sample time of the PID needs to be looked at for this application it's to fast adjusting the power output to fast, this might be caused be the cycle time of 
the MCU running at 10ms. Even though the temperature and humidity reading only occurs every 2s.

Here is a list of used Breakout Boards and components; 
  - DHT22 temp/humidity sensor 
  - arduino MEGA Board
  - 16x2 LCD with I2C converter https://arduinogetstarted.com/tutorials/arduino-lcd-i2c
  - Joystick breakout board with variable resistance x-axis and y-axis https://arduinogetstarted.com/tutorials/arduino-joystick
  - Robotdyn Dimmer https://robotdyn.com/ac-light-dimmer-module-1-channel-3-3v-5v-logic-ac-50-60hz-220v-110v.html
  - ultrasonic mist generator https://www.otronic.nl/nl/ultrasone-mist-maker-damper-luchtbevochtiger.html
  - Relay https://arduinogetstarted.com/tutorials/arduino-relay

The Relay was used to bridge the button on the mist generator, and replaces the manual use of the button. the mist generator button has 3 functions cycling through by short pushes. A long push will start the mist generator and stop after releasing, so there is a minimum of time needed for the relay to latch to activate this function. Otherwise you need to make the function a tri state function namely;  "On / pulsating / Off". For the ease and not really necessary tri state function, I just made it pulsate by bridging the button long enough to make use of the fourth function on the board : "On while you press the button, Off after releasing the button"

it probably is possible to make your own High frequency board with custom piezo element if anyone ever does, feel free to share it i would love to see the design and code.


I will try to make a electrical schematic soon with a link in this description, for now you can use the I/O of the PINs as reference to recreating this arduino setup
