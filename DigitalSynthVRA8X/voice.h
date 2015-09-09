#include "common.h"

template <uint8_t T>
class Voice {
  static uint8_t m_note_number;

public:
  INLINE static void initialize() {
    IVCO<0>::initialize();
    IVCA<0>::initialize();
    IEG<0>::initialize();
    m_note_number = NOTE_NUMBER_MIN;
  }

  INLINE static void note_on(uint8_t note_number) {
    if ((note_number >= NOTE_NUMBER_MIN) && (note_number <= NOTE_NUMBER_MAX)) {
      m_note_number = note_number;
      IEG<0>::note_on();
    }
  }

  INLINE static void note_off() {
    IEG<0>::note_off();
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case OSC_TYPE:
      // TODO
      break;
    case OSC_COLOR:
      // TODO
      break;
    case MOD_FREQ:
      // TODO
      break;
    case MOD_DEPTH:
      // TODO
      break;
    case ENV_A:
      IEG<0>::set_attack(controller_value);
      break;
    case ENV_D:
      IEG<0>::set_decay_release(controller_value);
      break;
    case ENV_S:
      IEG<0>::set_sustain(controller_value);
      break;
    case ENV_R:
      // TODO
      break;
    case ALL_NOTES_OFF:
      IEG<0>::note_off();
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t  eg_output = IEG<0>::clock();
    int16_t  vco_output = IVCO<0>::clock(m_note_number << 8, eg_output, 0);
    int16_t  vca_output = IVCA<0>::clock(vco_output, eg_output);
    return high_sbyte(vca_output);
  }
};

template <uint8_t T> uint8_t Voice<T>::m_note_number;
