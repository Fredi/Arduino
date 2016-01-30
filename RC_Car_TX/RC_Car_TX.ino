#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define PIN_X A0
#define PIN_Y A1

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

struct {
  int X;
  int Y;
} data;

int xValue = 0;
int yValue = 0;

const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup() {
  Serial.begin(57600);
  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.setRetries(15,15);

  radio.openWritingPipe(pipe);

  radio.printDetails();
}

void loop() {
  xValue = analogRead(PIN_X);
  data.X = xValue;
  yValue = analogRead(PIN_Y);
  data.Y = yValue;

  if ((xValue < 506 || xValue > 510) || (yValue < 530 || yValue > 534)) {
    bool ok = radio.write(&data, sizeof(data));

    Serial.print("X: ");
    Serial.print(xValue);
    Serial.print(" Y: ");
    Serial.println(yValue);
  }
} // Loop

