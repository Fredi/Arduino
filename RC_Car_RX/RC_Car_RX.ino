#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define PIN_SPEED   3
#define PIN_FORWARD 4
#define PIN_REVERSE 5
#define PIN_SERVO   6

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

struct {
  int X;
  int Y;
} receivedData;

Servo servo;

int xValue = 0;
int yValue = 0;
byte speedValue = 0;
bool forward = true;

const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  pinMode(PIN_SPEED, OUTPUT);
  pinMode(PIN_FORWARD, OUTPUT);
  pinMode(PIN_REVERSE, OUTPUT);

  servo.attach(PIN_SERVO);

  Serial.begin(57600);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.setRetries(15,15);

  radio.openReadingPipe(1, pipe);
  radio.startListening();

  radio.printDetails();
}

void loop() {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&receivedData, sizeof(receivedData));
    }

    if (receivedData.X != xValue) {
      xValue = receivedData.X;
      servo.write(map(xValue, 0, 1023, 135, 45));
    }

    if (receivedData.Y != yValue) {
      yValue = receivedData.Y;
      if (yValue >= 512) {
        speedValue = map(yValue, 512, 1023, 0, 255);
        forward = true;
      } else {
        speedValue = map(yValue, 0, 511, 255, 0);
        forward = false;
      }

      digitalWrite(PIN_FORWARD, forward);
      digitalWrite(PIN_REVERSE, !forward);
      analogWrite(PIN_SPEED, speedValue);
    }

    Serial.print(receivedData.X);
    Serial.print(" - ");
    Serial.println(receivedData.Y);
  }
} // Loop

