#include <TinyGPS++.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <Button2.h>
#include <RTClib.h>

// Pin definitions
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define SD_CS_PIN 4
#define LED_PIN 22
#define BUTTON_PIN 14

TinyGPSPlus gps;
Button2 button;
RTC_DS3231 rtc;

bool isLogging = false;
File logFile;
String fileName;

// Function to get the current date in Sydney timezone
String getSydneyDate() {
  DateTime now = rtc.now();
  now = now + TimeSpan(36000); // Adjust for Sydney timezone (AEST, UTC +10)
  char buffer[30];
  sprintf(buffer, "%04d%02d%02d GPX Data.gpx", now.year(), now.month(), now.day());
  return String(buffer);
}

// Initialize the GPS module
void setupGPS() {
  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

// Initialize the SD card
bool setupSD() {
  if (!SD.begin(SD_CS_PIN)) {
    flashLED(128, 0, 128, 500); // Purple
    return false;
  }
  return true;
}

// Function to start logging
void startLogging() {
  if (!setupSD()) return;

  fileName = getSydneyDate();
  logFile = SD.open(fileName, FILE_WRITE);
  if (!logFile) {
    flashLED(128, 0, 128, 500); // Purple
    return;
  }

  logFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  logFile.println("<gpx version=\"1.1\" creator=\"ATOM GPS Logger\">");
  logFile.println("<trk><name>ATOM GPS Track</name><trkseg>");

  isLogging = true;
  flashLED(0, 128, 0, 500); // Green
}

// Function to stop logging
void stopLogging() {
  if (logFile) {
    logFile.println("</trkseg></trk></gpx>");
    logFile.close();
  }
  isLogging = false;
  flashLED(128, 0, 0, 500); // Red
}

// Function to log GPS data
void logGPSData() {
  if (!gps.location.isValid() || !gps.speed.isValid() || !gps.altitude.isValid()) return;

  String gpxData = "<trkpt lat=\"" + String(gps.location.lat(), 6) + "\" lon=\"" + String(gps.location.lng(), 6) + "\">";
  gpxData += "<ele>" + String(gps.altitude.meters()) + "</ele>";
  gpxData += "<speed>" + String(gps.speed.kmph()) + "</speed>";
  gpxData += "</trkpt>";

  if (logFile) {
    logFile.println(gpxData);
    logFile.flush();
    flashLED(0, 0, 128, 100); // Blue
  } else {
    flashLED(128, 0, 128, 500); // Purple
  }
}

// LED flashing function
void flashLED(int red, int green, int blue, int duration) {
  // Adjust the LED brightness here if needed
  analogWrite(LED_PIN, red);
  delay(duration);
  analogWrite(LED_PIN, green);
  delay(duration);
  analogWrite(LED_PIN, blue);
  delay(duration);
  analogWrite(LED_PIN, 0);
}

// Setup function
void setup() {
  pinMode(LED_PIN, OUTPUT);

  button.begin(BUTTON_PIN);
  button.setTapHandler([](Button2 &b) {
    if (isLogging) {
      stopLogging();
    } else {
      startLogging();
    }
  });

  button.setDoubleClickHandler([](Button2 &b) {
    stopLogging();
    // Code to shut down the device if applicable
  });

  setupGPS();
  Wire.begin();
  rtc.begin();

  startLogging(); // Start logging automatically when the device is powered on
}

// Main loop function
void loop() {
  button.loop();

  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  if (isLogging) {
    logGPSData();
  }
}
