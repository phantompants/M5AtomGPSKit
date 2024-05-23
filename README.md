# M5AtomGPSKit
Hi
I am trying to get a simple GPS logger function working so I can geolocate photos and utilize elevation/speed info in GoPro videos when I'm travelling.

I bought the M5Stack ATOM GPS Kit, and have tried numerous coding attempts, including using ChatGPT to assist, but can't get it to do what I need.

Current code is here https://github.com/phantompants/M5AtomGPSKit/blob/main/M5-AtomicGPS-ExampleCode-GPX-Elevation-ChatGPT-Coding.ino 

ChatGPT (Code Pilot) Prompt:
Need code to make a GPS logger with an ATOM GPS Development Kit (M8030-KT), ensure it logs GPS data as a GPX compatible file, including Latitude, Longitude, Elevation and Speed. 

Ensure it is configured for the southern hemisphere. 

Using Arduino IDE 2.3.2.

Segment and name the GPS files into days based on Sydney Australia time in format "YYYYMMDD GPX Data.gpx"

Enable logging on power up and use the Atom Lite button click:
- For on and initialising - Single Click enable 
- For shutdown and power-off - Double click and show a red LED on the Atom Lite.
Enable Atom Lite LED
- For on and initialising - show a green LED on the Atom Lite
- For shutdown and power-off - show a green LED on the Atom Lite
- When receiving GPS data and writing to the SD Card Flash the Atom Lite LED blue
- When writing to the SD Card is failed or full Flash the Atom Lite LED purple. 
