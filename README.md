# Push Start

```

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
  |---+---+---+---|
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
  
```
