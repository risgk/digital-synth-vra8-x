#include "common.h"

template <uint8_t T>
class Voice {
  static uint8_t m_note_number;

public:
  INLINE static void initialize() {
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEnv<0>::initialize();
    m_note_number = NOTE_NUMBER_MIN;
  }

  INLINE static void note_on(uint8_t note_number) {
    if ((note_number >= NOTE_NUMBER_MIN) && (note_number <= NOTE_NUMBER_MAX)) {
      m_note_number = note_number;
      IEnv<0>::note_on();
    }
  }

  INLINE static void note_off() {
    IEnv<0>::note_off();
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
      IFilter<0>::set_cutoff(controller_value);
      break;
    case LPF_RESONANCE:
      IFilter<0>::set_resonance(controller_value);
      break;
    case ENV_A:
      IEnv<0>::set_attack(controller_value);
      break;
    case ENV_D:
      // TODO
      IEnv<0>::set_decay_release(controller_value);
      break;
    case ALL_NOTES_OFF:
      IEnv<0>::note_off();
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t  env_output = IEnv<0>::clock();
    int16_t  osc_output = IOsc<0>::clock(m_note_number << 8, env_output, 0);
    int16_t  filter_output = IFilter<0>::clock(osc_output, env_output);
    int16_t  amp_output = IAmp<0>::clock(filter_output, env_output);
    return high_sbyte(amp_output);
  }
};

template <uint8_t T> uint8_t Voice<T>::m_note_number;
