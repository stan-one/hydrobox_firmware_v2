# Project Hydrobox

This project aims to create an all-in-one solution for microscale hydroponic cultivation.

There are 3 repositories, 1 for the firwmare, 1 for the hardware and 1 for CAD files.

This project is based on a ESP32C3 microcontroller programed using the ESP-IDF and FreeRTOS. The front-end was done with NodeRED. For the hardware kicad was used and for the 3d desing Solidworks was used.

This project is the byproduct of what once was intended to be a closed source commercial product that fell into oblivium. 






## Authors

- [@stan-one](https://www.github.com/stan-one)


## FAQ

#### Is this a ready to build and use project

No, I haven't built the PCB yet nor I have ordered the PMMA enclosure. Although about the software I'm pretty confident that is almost ready to ship as this firmware is a something like a V2 of a commercial product. The firmware here has been somewhat downgraded.  

#### What else do I need to build it 


Just a pH probe and calibration liquids.



## Documentation

Doxygen files included in the /doc/ folders.

## Deployment

You need to install NodeRED and PlatformIO

0) Open the project on PlatformIO
1) In the config.h file set your wifis name and password
2) Load the hydrobox.js file from the ui folder in NodeRED


**TO BE FIXED** 

In the http nodes in NodeRED change the url adress to your local IP.
Soon I will upload how to use mDNS to solve this issue.
