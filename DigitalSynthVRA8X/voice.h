#include "common.h"

template <uint8_t T>
class Voice {
  static uint8_t m_note_number;

public:
  INLINE static void initialize() {
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEG<0>::initialize();
    ILFO<0>::initialize();
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
    case OSC_MODE:
      // TODO
      break;
    case OSC_COLOR:
      // TODO
      break;
    case MOD_RATE:
      // TODO
      break;
    case MOD_DEPTH:
      // TODO
      break;
    case LPF_CUTOFF:
      // TODO
      break;
    case LPF_RESONANCE:
      // TODO
      break;
    case ENV_A:
      IEG<0>::set_attack(controller_value);
      break;
    case ENV_D:
      IEG<0>::set_decay_release(controller_value);
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t eg_output     = IEG<0>::clock();
    int8_t  lfo_output    = ILFO<0>::clock(eg_output);
    int16_t osc_output    = IOsc<0>::clock(m_note_number, eg_output, lfo_output);
    int16_t filter_output = IFilter<0>::clock(osc_output, eg_output);
    int16_t amp_output    = IAmp<0>::clock(filter_output, eg_output);
    return high_sbyte(amp_output);
  }
};

template <uint8_t T> uint8_t Voice<T>::m_note_number;
