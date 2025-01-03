# Automatic Garden System

This project implements an automatic garden management system. 
The software is based on Platformio and Arduino and allows you to adapt the code to different 
microcontrollers. The system uses various kinds of sensors such as the soil, air moisture sensor, 
as well as the temperature sensor. The controller, in turn, connects to Wi-Fi and provides 
the ability to receive time via the Internet. Time is also set in a real-time sensor due to which, 
in the absence of an Internet connection, the system works correctly. Interaction over time is 
necessary in order to automatically turn off and turn on the phytolamp (grow light) lighting 
system, for the correct observance of light phases. In the system, it is subsequently planned 
to implement the sending of notifications via Telegram Bot, as well as to be able to obtain 
the current states of the system.

Currently supported ESP8266.