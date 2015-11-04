#pragma once

#include "common.h"
#include "mul-q.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  friend class OscModePulseSaw;

  static uint16_t m_phase_0;  // LFO or MOD Signal
  static uint16_t m_phase_1;
  static uint16_t m_phase_2;
  static uint16_t m_phase_3;

  static uint8_t  m_mode;
  static uint8_t  m_color;
  static uint16_t m_mod_rate;
  static uint16_t m_mod_depth;

public:
  INLINE static void initialize() {
    m_phase_0 = 0;
    m_phase_1 = 0;
    m_phase_2 = 0;
    m_phase_3 = 0;
    m_mode = 0;
    set_mode(0);
    set_color(0);
    set_mod_rate(0);
    set_mod_depth(0);
  }

  INLINE static void set_mode(uint8_t controller_value) {
    uint8_t mode = (static_cast<uint8_t>(controller_value + 8) >> 4) + 1;
    if (m_mode != mode) {
      m_mode = mode;
      reset_phase();
    }
  }

  INLINE static void set_color(uint8_t controller_value) {
    m_color = controller_value;
  }

  INLINE static void set_mod_rate(uint8_t controller_value) {
    switch (m_mode) {
    case OSC_MODE_1_FM:
      {
        uint8_t old_fm_ratio = mod_rate_to_fm_ratio(m_mod_rate);
        uint8_t new_fm_ratio = mod_rate_to_fm_ratio(controller_value);
        if (old_fm_ratio != new_fm_ratio) {
          reset_phase();
        }
      }
      break;
    }

    m_mod_rate = controller_value;
  }

  INLINE static void set_mod_depth(uint8_t controller_value) {
    m_mod_depth = controller_value << 1;
  }

  INLINE static int16_t clock(uint8_t pitch_control, uint8_t mod_eg_control) {
    int16_t result = 0;

    switch (m_mode) {
    case OSC_MODE_1_FM:
      {
        const uint8_t* wave_table = g_osc_sin_wave_table_h1;
        uint16_t freq = g_osc_freq_table[pitch_control - NOTE_NUMBER_MIN];
        uint16_t freq_d = freq + (((m_color + 1) >> 2) << 1);
        uint16_t mod_freq = freq * mod_rate_to_fm_ratio(m_mod_rate);
        uint16_t mod_freq_d = freq_d * mod_rate_to_fm_ratio(m_mod_rate);

        m_phase_2 += mod_freq;
        m_phase_3 += mod_freq_d;
        m_phase_0 += freq;
        m_phase_1 += freq_d;

        const uint8_t* saw_wave_table = g_osc_saw_wave_tables[pitch_control - NOTE_NUMBER_MIN];

        int8_t wave_2 = +get_wave_level(saw_wave_table, m_phase_2);
        int8_t wave_3 = +get_wave_level(saw_wave_table, m_phase_3);
        int8_t wave_0 = +get_wave_level(wave_table, m_phase_0 + ((wave_2 * high_byte(m_mod_depth * mod_eg_control)) << 1));
        int8_t wave_1 = +get_wave_level(wave_table, m_phase_1 + ((wave_3 * high_byte(m_mod_depth * mod_eg_control)) << 1));

        int16_t mixed = wave_0 * 160 + wave_1 * 80;
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_3_SAW_XOR:
      {
        // TODO
      }
      break;
    case OSC_MODE_5_PWM_SAW:
      {
        int8_t mod_lfo_control = triangle_lfo_clock(mod_eg_control, m_mod_rate);

        const uint8_t* wave_table = g_osc_saw_wave_tables[pitch_control - NOTE_NUMBER_MIN];
        uint16_t freq = g_osc_freq_table[pitch_control - NOTE_NUMBER_MIN];
        m_phase_1 += freq;

        uint16_t shift_lfo = (mod_lfo_control * m_mod_depth);
        int8_t saw_down      = +get_wave_level(wave_table, m_phase_1);
        int8_t saw_up        = -get_wave_level(wave_table, m_phase_1 + (128 << 8) - shift_lfo);
        int8_t saw_down_copy = +get_wave_level(wave_table, m_phase_1              + shift_lfo);

        int16_t mixed = saw_down      * 127 +
                        saw_up        * static_cast<uint8_t>(127 - m_color) +
                        saw_down_copy * m_color;
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_7_SYNC_FM:
      {
        // TODO
      }
      break;
    case OSC_MODE_9_RING_PWM:
      {
        // TODO
      }
      break;
    }

    return result;
  }

private:
  INLINE static void reset_phase() {
    m_phase_0 = 0;
    m_phase_1 = 0;
    m_phase_2 = 0;
    m_phase_3 = 0;
  }

  INLINE static int8_t mod_rate_to_fm_ratio(uint8_t mod_rate) {
    return ((m_mod_rate + 8) >> 4) + 1;
  }

  INLINE static int8_t triangle_lfo_clock(uint8_t mod_eg_control, uint8_t mod_rate) {
    m_phase_0 += ((mod_rate + 1) >> 1) + 1;

    uint16_t level = m_phase_0;
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte(high_sbyte(level << 1) * mod_eg_control);
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, uint16_t phase) {
    uint8_t curr_index = high_byte(phase);
    uint16_t tmp = pgm_read_word(wave_table + curr_index);
    int8_t curr_data = low_byte(tmp);
    int8_t next_data = high_byte(tmp);
    uint8_t next_weight = low_byte(phase);

    // lerp
    int8_t level = curr_data + high_sbyte((next_data - curr_data) * next_weight);

    return level;
  }
};

template <uint8_t T> uint16_t Osc<T>::m_phase_0;
template <uint8_t T> uint16_t Osc<T>::m_phase_1;
template <uint8_t T> uint16_t Osc<T>::m_phase_2;
template <uint8_t T> uint16_t Osc<T>::m_phase_3;
template <uint8_t T> uint8_t  Osc<T>::m_mode;
template <uint8_t T> uint8_t  Osc<T>::m_color;
template <uint8_t T> uint16_t Osc<T>::m_mod_rate;
template <uint8_t T> uint16_t Osc<T>::m_mod_depth;
