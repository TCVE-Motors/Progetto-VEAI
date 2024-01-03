# VEAI Project (Progetto VEAI)

Progetto VEAI (Intelligent Artisan Electric Vehicle), consists of a compact single-seater, renewable, 100% Open-Source and electric vehicle for the good of humanity developed by Ferrara Davide Giacomo and his TCVE Motors team.


## Acknowledgments

 - [TCVE Motors official website] ( https://tcvemotors.webflow.io/ )
 - [VEAI Project post] (https://tcvemotors.webflow.io/progetti/progettoveai)


## Authors

- [Authors] [Ferrara Davide] ( https://github.com/FerraraDavideG )


## Full Documentation

[Fulll Documentation] (https://drive.google.com/file/d/1WIrQb47PrjLjpMIPrqZszVo9VQTzwGDa/view)


## License

Software: (Files like "veai.ino" and "VEAIgui") [GNU GPL-3.0] [ https://www.gnu.org/licenses/gpl-3.0.html#license-text ]


## Attribution

This project uses the following components:

- VEVOR MY1020D (BLDC Motor 48V 2000W 2.67hp)
- VEVOR BY15WF02-A (ECU for MY120D engine)
- Arduino Uno R4
- Raspberry Pi 3B+

The specific code for Arduino and the computer are based on the project by Ferrara Davide Giacomo.

For further details on licenses and attributions, please see the relevant source files.


## Wiring


The component connections are configured as follows:

- The DHT-22 temperature and humidity sensor is connected to pin 4 of the Arduino.
- The KY-026 flame sensor is connected to pin 5 of the Arduino.
- The MQ-135 gas sensor is connected to pin A0 of Arduino.
- The KY-035 hall effect sensor is connected to pin 3 of the Arduino.
- The ultrasonic sensor HCSR04 is connected to pins 7 and 8 of the Arduino.
- TMP36GT9Z sensors are connected to pins A2 & A4 for Motor and Motor Battery respectively on Arduino.
- The buzzer is connected to pin 10 of the Arduino.
- Arduino is connected via USB cable for serial communication to the Raspberry pi.
- Raspberry Pi is connected to the 10' Touchscreen via HDMI for multimedia and USB for touch and power.
- To be continued!


## Feedback

We are always open to feedback and talking to the community.

You can contact us via email at: helptcvemotors@gmail.com or through our website at https://tcvemotors.webflow.io/contact

Please do not abuse our contact services!
