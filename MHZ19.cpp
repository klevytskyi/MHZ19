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

#include <MHZ19.h>

/*
 * Checks a crc of an 9 bype packet
 */

byte MHZ19::getCheckSum(byte* packet) {
  byte checksum = 0;
  for(uint8_t i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}

byte MHZ19::CMD_READ[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Read command
byte MHZ19::CMD_CALIBRATE_ZERO[9] = {0xFF, 0x01, 0x87, 0x00, 0x00,0x00, 0x00, 0x00, 0x78}; // Calibrate ZERO point
byte MHZ19::CMD_ABC_ON[9] = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6}; // Enables Automatic Baseline Correction
byte MHZ19::CMD_ABC_OFF[9] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}; // Disables Automatic Baseline Correction

/*
 * Sends command to MH-Z19 with back crc checking
 * @param 9 bytes that needed to be send
 * @return bool indicating command receive
 */

bool MHZ19::sendCmd(byte cmd[9]) {
  unsigned char response[9];

  _streamRef->write(cmd, 9);
  _streamRef->readBytes(response, 9);

  byte crc = getCheckSum(response);

  if (response[0] == 0xFF && response[1] == cmd[2] && response[8] == crc) {
    return true;
  }

  return false;
}

/*
 * @param Stream instance reference. Usually HardwareSerial or SoftwareSerial
 */

void MHZ19::setSerial(Stream *streamRef) {
  _streamRef = streamRef;
};

/*
 * Sets CO2 detection ranges, 0-2000ppm or 0-5000ppm.
 * @param range top from 1000 to 5000
 * Setting lower than 1000 is unavaliable.
 * NOTE: Datasheet recomends usage of 0-2000ppm and 0-5000ppm detection ranges.
 */

bool MHZ19::setRange(int range) {
  if (!(range >= 1000 && range <= 5000)) {
    return false;
  }

  byte range6thByte = range >> 8;
  byte range7thByte = range & 0xFA;
  byte rangeCmd[9] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, range6thByte, range7thByte, 0x00};
  rangeCmd[8] = getCheckSum(rangeCmd);

  return sendCmd(rangeCmd);
}

/*
 * Enables sensor Automatic Baseline Correction (ABC)
 */

bool MHZ19::enableABC() {
  return sendCmd(CMD_ABC_ON);
}

/*
 * Disables sensor Automatic Baseline Correction (ABC)
 */

bool MHZ19::disableABC() {
  return sendCmd(CMD_ABC_OFF);
}

/*
 * Calibrates sensor's zero point assuming that current value is 400ppm
 */

bool MHZ19::calibrateZero() {
  return sendCmd(CMD_CALIBRATE_ZERO);
}

/*
 * Calibrates sensor's span point (have no idea what it is)
 */

bool MHZ19::calibrateSpan(int span) {
  if (!(span >= 1000 && span <= 5000)) {
    return false;
  }

  byte span6thByte = span >> 8;
  byte span7thByte = span & 0xFA;
  byte spanCmd[9] = {0xFF, 0x01, 0x88, 0x00, 0x00, 0x00, span6thByte, span7thByte, 0x00};
  spanCmd[8] = getCheckSum(spanCmd);

  return sendCmd(spanCmd);
}

/*
 * Read current ppm from sensor
 */

int MHZ19::readValue() {
  unsigned int co2 = -1;
  unsigned char response[9];
  _streamRef->write(CMD_READ, 9);

  if (_streamRef->available()) {
    _streamRef->readBytes(response, 9);

    byte crc = getCheckSum(response);

    if (response[0] == 0xFF && response[1] == CMD_READ[2] && response[8] == crc) {
      unsigned int responseHigh = (unsigned int) response[2];
      unsigned int responseLow = (unsigned int) response[3];
      unsigned int ppm = (256*responseHigh) + responseLow;
      co2 = ppm;
    }
  }

  return co2;
}

/*
 * Returns if sensor is ready to use and sends real data.
 * According to datasheet MH-Z19 preheat time is 3 mins.
 */

bool MHZ19::isReady() {
  if (!sensorIsReady) {
    if (millis() > PREHEAT_TIME) {
      sensorIsReady = true;
      return sensorIsReady;
    }
    return false;
  }
  return sensorIsReady;
}
