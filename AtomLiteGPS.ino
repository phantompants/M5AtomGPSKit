#include <M5Atom.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BluetoothSerial.h>

// Constants and Pins
#define GPS_TX_PIN 22
#define GPS_RX_PIN 21
#define BUTTON_PIN 39
#define SD_CS_PIN 4
#define DATA_PIN    23 // Example SPI data pin
#define CLOCK_PIN   18 // Example SPI clock pin

// Wi-Fi Credentials
const char* ssid = "Mumbo-Jumbo";
const char* password = "BPJY3Z5RF8@23";

// GPS and Bluetooth
TinyGPSPlus gps;
BluetoothSerial SerialBT;

// Wi-Fi and NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// States and Cities Data (Example)
struct City {
    String name;
    float latitude;
    float longitude;
};
City cities[] = {
    {"Sydney", -33.8688, 151.2093},
    {"Melbourne", -37.8136, 144.9631},
    {"Brisbane", -27.4698, 153.0251},
    {"Perth", -31.9505, 115.8605},
    {"Adelaide", -34.9285, 138.6007},
    {"Canberra", -35.2809, 149.1300},
    {"Hobart", -42.8821, 147.3272},
    {"Darwin", -12.4634, 130.8456},
    {"Auckland", -36.8485, 174.7633},
    {"Wellington", -41.2865, 174.7762},
    {"Christchurch", -43.5321, 172.6362}
};
const int numCities = sizeof(cities) / sizeof(cities[0]);

// Global Variables
bool loggingEnabled = false;
double lastLatitude = 0;
double lastLongitude = 0;
String currentFileName = "";

void setup() {
    M5.begin(true, false, true);  // Initialize M5Atom with the display and IMU disabled
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        handleError();
        return;
    }

    // Initialize Wi-Fi
    connectWiFi();

    // Initialize Bluetooth
    SerialBT.begin("M5Stack");

    // Initialize NTP
    timeClient.begin();
    timeClient.setTimeOffset(36000); // Adjust for your timezone

    setLEDColor(255, 0, 0); // Red LED: Power on
}

void loop() {
    M5.update();

    // Handle button press to toggle logging
    if (M5.Btn.wasPressed()) {
        toggleLogging();
    }

    // Read GPS data
    while (Serial2.available() > 0) {
        gps.encode(Serial2.read());
        if (gps.location.isUpdated()) {
            double latitude = gps.location.lat();
            double longitude = gps.location.lng();
            double elevation = gps.altitude.meters();
            timeClient.update();
            String timeStamp = timeClient.getFormattedTime();

            if (checkMovement(latitude, longitude)) {
                logData(latitude, longitude, elevation, timeStamp);
            }
        }
    }

    delay(1000);
}

void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

bool checkMovement(double latitude, double longitude) {
    double distance = TinyGPSPlus::distanceBetween(lastLatitude, lastLongitude, latitude, longitude);
    if (distance > 2) {
        lastLatitude = latitude;
        lastLongitude = longitude;
        return true;
    }
    return false;
}

void logData(double latitude, double longitude, double elevation, String timeStamp) {
    if (!loggingEnabled) return;

    String fileName = getNearestCity(latitude, longitude) + ".gpx";
    File file = SD.open("/" + fileName, FILE_APPEND);
    if (!file) {
        handleError();
        return;
    }

    file.printf("<trkpt lat=\"%f\" lon=\"%f\"><ele>%f</ele><time>%s</time></trkpt>\n", latitude, longitude, elevation, timeStamp.c_str());
    file.close();

    setLEDColor(0, 255, 0); // Green LED: Logging data
    delay(100);
    setLEDColor(0, 0, 0); // Turn off LED
}

String getNearestCity(double latitude, double longitude) {
    double minDistance = 1e12;
    String nearestCity = "";
    for (int i = 0; i < numCities; i++) {
        double distance = TinyGPSPlus::distanceBetween(latitude, longitude, cities[i].latitude, cities[i].longitude);
        if (distance < minDistance) {
            minDistance = distance;
            nearestCity = cities[i].name;
        }
    }
    return nearestCity;
}

void toggleLogging() {
    loggingEnabled = !loggingEnabled;
    if (loggingEnabled) {
        setLEDColor(0, 0, 255); // Blue LED: Logging enabled
    } else {
        setLEDColor(0, 0, 0); // Turn off LED
    }
}

void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    M5.dis.fillpix(CRGB(r, g, b));
}

void handleError() {
    while (true) {
        setLEDColor(255, 0, 0); // Red LED: Error
        delay(500);
        setLEDColor(0, 0, 0); // Turn off LED
        delay(500);
    }
}
