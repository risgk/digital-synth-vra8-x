#pragma once

#include "common.h"
#include "mul-q.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
   static const uint8_t* m_wave_table;
   static uint16_t       m_phase;
   static uint8_t        m_mix;
   static uint8_t        m_mix_eg_amt;
   static uint16_t       m_pulse_width;
   static uint16_t       m_saw_shift;
   static uint8_t        m_color_lfo_amt;

public:
  INLINE static void initialize() {
    m_wave_table = NULL;
    m_phase = 0;
    set_mix(0);
    set_mix_eg_amt(0);
    set_pulse_width(0);
    set_saw_shift(0);
    set_color_lfo_amt(0);
  }

  INLINE static void set_mix(uint8_t controller_value) {
    m_mix = controller_value;
  }

  INLINE static void set_mix_eg_amt(uint8_t controller_value) {
    m_mix_eg_amt = controller_value;
  }

  INLINE static void set_pulse_width(uint8_t controller_value) {
    m_pulse_width = (controller_value + 128) << 8;
  }

  INLINE static void set_saw_shift(uint8_t controller_value) {
    m_saw_shift = controller_value << 8;
  }

  INLINE static void set_color_lfo_amt(uint8_t controller_value) {
    m_color_lfo_amt = controller_value << 1;
  }

  INLINE static int16_t clock(uint16_t pitch_control, uint8_t mod_eg_control,
                                                      int8_t mod_lfo_control) {
    uint8_t coarse_pitch = high_byte(pitch_control);
    uint8_t fine_pitch = low_byte(pitch_control);

    m_wave_table = NULL;
    uint16_t freq = mul_q16_q16(g_osc_freq_table[coarse_pitch - (NOTE_NUMBER_MIN - 1)],
                                g_osc_tune_rate_table[fine_pitch >>
                                                      (8 - OSC_TUNE_RATE_TABLE_STEPS_BITS)]);
    m_phase += freq;

    uint16_t shift_lfo = (mod_lfo_control * m_color_lfo_amt);
    int8_t saw_down      = +high_byte(m_phase);
    int8_t saw_up        = -high_byte(m_phase + m_pulse_width - shift_lfo);
    int8_t saw_down_copy = +high_byte(m_phase + m_saw_shift   + shift_lfo);

    uint8_t mix = m_mix + high_byte(m_mix_eg_amt * mod_eg_control);
    if (mix > 127) {
      mix = 127;
    }
    int16_t mixed = saw_down      * 127 +
                    saw_up        * static_cast<uint8_t>(127 - mix) +
                    saw_down_copy * mix;

    return mixed >> 1;
  }

private:
  // todo
};

template <uint8_t T> const uint8_t* Osc<T>::m_wave_table;
template <uint8_t T> uint16_t       Osc<T>::m_phase;
template <uint8_t T> uint8_t        Osc<T>::m_mix;
template <uint8_t T> uint8_t        Osc<T>::m_mix_eg_amt;
template <uint8_t T> uint16_t       Osc<T>::m_pulse_width;
template <uint8_t T> uint16_t       Osc<T>::m_saw_shift;
template <uint8_t T> uint8_t        Osc<T>::m_color_lfo_amt;
