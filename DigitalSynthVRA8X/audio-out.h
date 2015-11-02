// refs http://playground.arduino.cc/Code/PCMAudio

#pragma once

#include "common.h"

template <uint8_t T>
class AudioOut {
  static const int SPEAKER_PIN = 6;   // PD6 (OC0A)
  static const int LED_PIN     = 13;  // PB5

public:
  INLINE static void open() {
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(LED_PIN,     OUTPUT);

    // Timer/Counter0 (8-bit Fast PWM, Inverting, 62500 Hz)
    TCCR0A = 0x83;
    TCCR0B = 0x01;
    OCR0A  = 0x80;
  }

  INLINE static void write(int8_t level) {
    if (TIFR0 & _BV(TOV0)) {
      // overload
      PORTB |= _BV(5);
    } else {
      PORTB &= ~_BV(5);
    }
    while ((TIFR0 & _BV(TOV0)) == 0);
    TIFR1 = _BV(TOV0);
    OCR0A = 0x80 - level;
  }
};
