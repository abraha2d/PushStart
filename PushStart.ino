/*

           Push Start
   Arduino-based Keyless Ignition
   for 97-01 Toyota Camry (XV20)
     Designed by Kevin Abraham

*/

#include <AnalogTouch.h>

#define DEBUG 0

#define BUTTON_PRESS_THRESHOLD 500

#define pinACC 2
#define pinIG1 4
#define pinIG2 7
#define pinST2 8

#define pinCap A4
#define pinLed 10

// Slow down the automatic calibration cooldown
#define offset 2 // <= 6

byte mode = 0;
// 0 = off
// 1 = accesory
// 2 = ignition on
// 3 = engine start

bool pressed = false;
unsigned long touchStart = 0;

byte ledAmp = 0;
byte ledShift = 0;
int ledPeriod = 0;
unsigned long fadeStart = 0;

void setup() {

  // Bring relay pins low as soon as possible
  digitalWrite(pinACC, HIGH);
  digitalWrite(pinIG1, HIGH);
  digitalWrite(pinIG2, HIGH);
  digitalWrite(pinST2, HIGH);
  pinMode(pinACC, OUTPUT);
  pinMode(pinIG1, OUTPUT);
  pinMode(pinIG2, OUTPUT);
  pinMode(pinST2, OUTPUT);

  Serial.begin(57600);

  Serial.println();
  Serial.println("            Push Start            ");
  Serial.println("  Arduino-based Keyless Ignition  ");
  Serial.println("  for 97-01 Toyota Camry (XV20)   ");
  Serial.println("    Designed by Kevin Abraham     ");
  Serial.println();

  Serial.println("Mode: off");
  Serial.flush();

}

void OFF() {
  mode = 0;
  digitalWrite(pinACC, HIGH); // 2-3
  digitalWrite(pinIG1, HIGH); // 2-4
  digitalWrite(pinIG2, HIGH); // 7-6
  digitalWrite(pinST2, HIGH); // 7-8
  ledShift = 48; ledAmp = 0;
  Serial.println("Mode: off"); Serial.flush();
}

void ACC() {
  mode = 1;
  digitalWrite(pinACC, LOW);  // 2-3
  digitalWrite(pinIG1, HIGH); // 2-4
  digitalWrite(pinIG2, HIGH); // 7-6
  digitalWrite(pinST2, HIGH); // 7-8
  ledShift = 96; ledAmp = 64;
  ledPeriod = 2000; fadeStart = millis();
  Serial.println("Mode: accessory"); Serial.flush();
}

void ON() {
  mode = 2;
  digitalWrite(pinACC, LOW);  // 2-3
  digitalWrite(pinIG1, LOW);  // 2-4
  digitalWrite(pinIG2, LOW);  // 7-6
  digitalWrite(pinST2, HIGH); // 7-8
  ledShift = 32; ledAmp = 0;
  Serial.println("Mode: ignition on"); Serial.flush();
}

void ST() {
  mode = 3;
  digitalWrite(pinACC, LOW); // 2-3
  digitalWrite(pinIG1, LOW);  // 2-4
  digitalWrite(pinIG2, LOW);  // 7-6
  digitalWrite(pinST2, LOW);  // 7-8
  ledShift = 128; ledAmp = 127;
  ledPeriod = 500; fadeStart = millis();
  Serial.println("Mode: engine start"); Serial.flush();
}

void cycle() {
  mode += 1;
  switch (mode) {
    case 3:
      OFF(); break;
    case 1:
      ACC(); break;
    case 2:
      ON(); break;
  }
}

void loop() {

  uint16_t value = analogTouchRead(pinCap, 100);

  // Self calibrate capacitive touch
  static uint16_t ref = 0xFFFF;
  if (value < (ref >> offset))
    ref = (value << offset);
  else if (value > (ref >> offset))
    ref++;

  bool touched = (value - (ref >> offset)) > 100;

  if (touched & !pressed) {
    touchStart = millis();
  } else if (!touched & pressed) {
    if (mode == 3) {
      ON();
    } else if (millis() - touchStart < BUTTON_PRESS_THRESHOLD) {
      cycle();
    }
  }

  pressed = touched;

  if (pressed & millis() - touchStart > BUTTON_PRESS_THRESHOLD & mode != 3) {
    mode = 3;
    ST();
  }

  analogWrite(pinLed, ledShift + ledAmp * -cos(2 * PI / ledPeriod * (millis() - fadeStart)));

  delay(50);

}

