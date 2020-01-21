# WiFi-Air-Conditioner
Software end of AC unit

<img src="https://i.imgur.com/XBztfJC.jpg" height="350" width="350"> <img src="https://i.imgur.com/r7gg6zu.jpg" height="350" width="415">

## Overview
This project is a portable WiFi controlled air conditioner with a working Windows desktop application that is able to perform basic 
functionalities, such as toggling power or changing the temperature. It utilizes the Arduino Uno WiFi Rev2, as well as various other
modules for key functionalities.

## How it Works
During the first time setup, the AC unit will create an access point for the user to connect to, in which he/she will be brought to a
web server that scans for nearby networks. Next, the user will select and connect to the desired network, communicating to the Arduino
through HTTP requests. 

Once it successfully connects to a network, it will display the local IP of the Arduino WiFi shield, in which you
must input into the desktop application. The Arduino creates a local web server with various HTTP responses which interact with the
physical unit in some way. Through the desktop application, the user then sends HTTP requests for these various functions.

## Stage of Project
The core functionalities are complete, but there are several improvements to be made, such as:
- Device to cloud implementation to allow for remote communcation (outside of WiFi network)
- Captive portal for first time setup
- Save WiFi networks in EEProm and hash passwords for increased security
- Create SQL database of previously logged IP addresses
- Allow user to set times of when unit is turned on automatically
