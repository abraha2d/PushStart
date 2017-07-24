/*

           Push Start
   Arduino-based Keyless Ignition
   for 97-01 Toyota Camry (XV20)
     Designed by Kevin Abraham

   Operation Instructions:
   - Press to cycle through (OFF -> ACC -> ON -> OFF).
   - Press and hold at any time to crank engine.

  Technical details:

  Ignition Switch Harness:
   ---------------
  | 8 | 7 | 6 | X |
  |---------------|
  | 4 | 3 | 2 | X |
   ---------------
        \---/

  Pins:
  2: (Black) AM1
  3: (Black) ACC
  4: (Blk/Y) IG1
  6: (Blue ) IG2
  7: (Blue ) AM2
  8: (Red  ) ST2

  Ignition sequence:
  ACC = 2-3
  ON = 2-3, 2-4, 7-6
  ST = 2-4, 7-6, 7-8

*/

#include <AnalogTouch.h>

#define DEBUG 0

#define BUTTON_PRESS_THRESHOLD 500

#define pinCap A4

#define pinACC 2
#define pinIG1 4
#define pinIG2 7
#define pinST2 8

// Slow down the automatic calibration cooldown
#define offset 2 // <= 6

byte mode = 0;
// 0 = off
// 1 = accesory
// 2 = ignition on
// 3 = engine start

bool pressed = false;
unsigned long touchStart = 0;

void setup() {

  // Bring relay pins low as soon as possible
  pinMode(pinACC, OUTPUT);
  digitalWrite(pinACC, LOW);
  pinMode(pinIG1, OUTPUT);
  digitalWrite(pinIG1, LOW);
  pinMode(pinIG2, OUTPUT);
  digitalWrite(pinIG2, LOW);
  pinMode(pinST2, OUTPUT);
  digitalWrite(pinST2, LOW);

  Serial.begin(115200);

  Serial.println();
  Serial.println(" Push Start ");
  Serial.println(" Version 2.0 ");
  Serial.println();
  Serial.println(" Designed by Kevin Abraham ");
  Serial.println(" For 97-01 Toyota Camry (XV20) ");
  Serial.println();

  Serial.println("Mode: off");
  Serial.flush();

}

void OFF() {
  digitalWrite(pinACC, LOW);  // 2-3
  digitalWrite(pinIG1, LOW);  // 2-4
  digitalWrite(pinIG2, LOW);  // 7-6
  digitalWrite(pinST2, LOW);  // 7-8
  Serial.println("Mode: off"); Serial.flush();
}

void ACC() {
  digitalWrite(pinACC, HIGH); // 2-3
  digitalWrite(pinIG1, LOW);  // 2-4
  digitalWrite(pinIG2, LOW);  // 7-6
  digitalWrite(pinST2, LOW);  // 7-8
  Serial.println("Mode: accessory"); Serial.flush();
}

void ON() {
  digitalWrite(pinACC, HIGH); // 2-3
  digitalWrite(pinIG1, HIGH); // 2-4
  digitalWrite(pinIG2, HIGH); // 7-6
  digitalWrite(pinST2, LOW);  // 7-8
  Serial.println("Mode: ignition on"); Serial.flush();
}

void ST() {
  digitalWrite(pinACC, LOW);  // 2-3
  digitalWrite(pinIG1, HIGH); // 2-4
  digitalWrite(pinIG2, HIGH); // 7-6
  digitalWrite(pinST2, HIGH); // 7-8
  Serial.println("Mode: engine start"); Serial.flush();
}

void loop() {

  uint16_t value = analogTouchRead(pinCap);

  // Self calibrate capacitive touch
  static uint16_t ref = 0xFFFF;
  if (value < (ref >> offset))
    ref = (value << offset);
  else if (value > (ref >> offset))
    ref++;

  bool touched = (value - (ref >> offset)) > 40;

  if (touched & !pressed) {

    touchStart = millis();

  } else if (!touched & pressed) {

    if (mode == 3) {
      mode = 2;
    } else if (millis() - touchStart < BUTTON_PRESS_THRESHOLD) {
      mode += 1;
    }

    switch (mode) {
      case 3:
        mode = 0;
      case 0:
        OFF(); break;
      case 1:
        ACC(); break;
      case 2:
        ON(); break;
    }

  }

  pressed = touched;

  if (touched & millis() - touchStart > BUTTON_PRESS_THRESHOLD & mode != 3) {
    mode = 3;
    ST();
  }

  delay(50);

}

