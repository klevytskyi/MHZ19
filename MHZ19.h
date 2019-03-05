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

#ifndef MHZ19_H
#define MHZ19_H

#include <Arduino.h>

class MHZ19 {
  static const int PREHEAT_TIME = 180000; // 3 mins

  private:
    bool sensorIsReady;
    Stream *_streamRef;
    static byte CMD_READ[];
    static byte CMD_CALIBRATE_ZERO[];
    static byte CMD_ABC_ON[];
    static byte CMD_ABC_OFF[];
    static byte getCheckSum(byte *packet);

    bool sendCmd(byte*);

  public:
    void setSerial(Stream *streamRef);
    bool setRange(int);

    bool enableABC();
    bool disableABC();
    bool calibrateZero();
    bool calibrateSpan(int);
    int readValue();
    bool isReady();
};

#endif // MHZ19_H
