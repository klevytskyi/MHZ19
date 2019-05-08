/*
 * Designed to work with Winsen MH-Z19B CO2 sensor
 * - https://www.winsen-sensor.com/products/ndir-co2-sensor/mh-z19b.html
 * According to:
 * - https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf
 *
 * This library implements methods for working with sensor via UART.
 *
 * Written by Kostiantyn Levytskyi <le.konstantos@gmail.com>
 *
 * MIT License
 *
 */
#include <Arduino.h>

// SW Serial
#define SERIAL_RX 13
#define SERIAL_TX 15

#include <SoftwareSerial.h>

SoftwareSerial swSerial(SERIAL_RX, SERIAL_TX, false, 256); // We are using SW serial in this example

#include <MHZ19.h>

MHZ19 mhz19;

void setup() {
    Serial.begin(9600);
    Serial.println("Testing of CO2 sensor");

    swSerial.begin(9600);

    co2.setStream(&swSerial); // pass swSerial to let it use it

    // ADVANCED USAGE NEXT:

    // if (!mhz19.setRange(5000)) { // Setting the range of 0-5000ppm
    //   Serial.println("RANGE WASN'T SET!");
    // } else {
    //   Serial.println("RANGE IS SET!");
    // }

    // if (!mhz19.disableABC(5000)) { // Automatic Baseline Correction
    //   Serial.println("ABC WASN'T DISABLED!");
    // } else {
    //   Serial.println("ABC WAS DISABLED!");
    // }

    // if (!mhz19.calibrateZero(5000)) { // Setting zero point
    //   Serial.println("ZERO POINT WASN'T SET!");
    // } else {
    //   Serial.println("ZERO POINT WAS SET!");
    // }
}


void loop() {
    if (co2.isReady()) { // Checking if sensor had preheated for 3 mins
      int co2ppm = co2.readValue(); // Reading CO2 value. (Returns -1 if response wasn't received)
      Serial.print("CO2: ");
      Serial.print(co2ppm);
      Serial.println("ppm");
    }
    delay(1000);
}
