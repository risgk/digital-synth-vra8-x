#pragma once

#include "common.h"
#include "mul-q.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  static const uint8_t* m_wave_table;
  static uint16_t       m_freq;
  static uint16_t       m_phase_lfo;
  static uint16_t       m_phase_0;
  static uint16_t       m_phase_1;
  static uint16_t       m_phase_2;
  static uint16_t       m_phase_3;
  static uint16_t       m_phase_4;
  static uint16_t       m_phase_5;
  static uint8_t        m_mode;
  static uint8_t        m_color;
  static uint16_t       m_mod_rate;
  static uint16_t       m_mod_depth;

public:
  INLINE static void initialize() {
    m_wave_table = g_osc_sine_wave_table_h1;
    m_freq = 1;
    reset_phase();
    set_mode(0);
    set_color(0);
    set_mod_rate(0);
    set_mod_depth(0);
  }

  INLINE static void set_mode(uint8_t controller_value) {
    uint8_t mode = (static_cast<uint8_t>(controller_value + 8) >> 4) << 4;
    if (mode >= 128) {
      mode = 127;
    }
    if (m_mode != mode) {
      m_mode = mode;
      reset_phase();
    }
  }

  INLINE static void set_color(uint8_t controller_value) {
    switch (m_mode) {
    case OSC_MODE_FM:
    case OSC_MODE_SYNC_FM:
      {
        uint8_t old_low_freq = value_to_low_freq(m_color);
        uint8_t new_low_freq = value_to_low_freq(controller_value);
        if (old_low_freq != 0 && new_low_freq == 0) {
          reset_phase();
        }
      }
      break;
    }

    m_color = controller_value;
  }

  INLINE static void set_mod_rate(uint8_t controller_value) {
    switch (m_mode) {
    case OSC_MODE_FM:
      {
        uint8_t old_fm_ratio = mod_rate_to_fm_ratio(m_mod_rate);
        uint8_t new_fm_ratio = mod_rate_to_fm_ratio(controller_value);
        if (old_fm_ratio != new_fm_ratio) {
          reset_phase();
        }
      }
      break;
    case OSC_MODE_SAW:
      {
        uint8_t old_low_freq = value_to_low_freq(m_mod_rate);
        uint8_t new_low_freq = value_to_low_freq(controller_value);
        if (old_low_freq != 0 && new_low_freq == 0) {
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

  INLINE static void note_on(uint8_t note_number) {
    m_wave_table = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
    m_freq = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
  }

  INLINE static int16_t clock(uint8_t mod_eg_control) {
    int16_t result = 0;

    switch (m_mode) {
    case OSC_MODE_FM:
      {
        uint16_t freq_detune = m_freq - value_to_low_freq(m_color);

        uint8_t fm_ratio = mod_rate_to_fm_ratio(m_mod_rate);
        uint16_t mod_freq = (m_freq >> 1) * fm_ratio;
        uint16_t mod_freq_detune = (freq_detune >> 1) * fm_ratio;

        m_phase_2 += mod_freq;
        m_phase_3 += mod_freq_detune;

        m_phase_0 += m_freq;
        m_phase_1 += freq_detune;

        int8_t wave_2 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_2);
        int8_t wave_3 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_3);

        int8_t wave_0 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_0 +
                          ((wave_2 * high_byte((m_mod_depth + 1) * mod_eg_control)) << 3));
        int8_t wave_1 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_1 +
                          ((wave_3 * high_byte((m_mod_depth + 1) * mod_eg_control)) << 3));

        int16_t mixed = wave_0 * 170 + wave_1 * 85;
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_SYNC_FM:
      {
        uint16_t freq_detune = m_freq - value_to_low_freq(m_color);
        m_phase_0 += m_freq;
        m_phase_1 += freq_detune;

        uint8_t fm_ratio = m_mod_rate + 4;
        int8_t wave_2 = get_wave_level(g_osc_sine_wave_table_h1,
                                       (uint16_t) ((m_phase_0 >> 3) * fm_ratio));
        int8_t wave_3 = get_wave_level(g_osc_sine_wave_table_h1,
                                       (uint16_t) ((m_phase_1 >> 3) * fm_ratio));

        int8_t wave_0 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_0 +
                          ((wave_2 * high_byte((m_mod_depth + 1) * mod_eg_control)) << 3));
        int8_t wave_1 = get_wave_level(g_osc_sine_wave_table_h1, m_phase_1 +
                          ((wave_3 * high_byte((m_mod_depth + 1) * mod_eg_control)) << 3));

        int16_t mixed = wave_0 * 170 + wave_1 * 85;
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_PWM_SAW:
      {
        int8_t mod_lfo_control = triangle_lfo_clock(mod_eg_control, m_mod_rate);
        int16_t shift_lfo = mod_lfo_control * m_mod_depth;

        m_phase_0 += m_freq;

        int8_t saw_down      = get_wave_level(m_wave_table, m_phase_0);
        int8_t saw_up        = get_wave_level(m_wave_table, m_phase_0 + (128 << 8) - shift_lfo);
        int8_t saw_down_copy = get_wave_level(m_wave_table, m_phase_0              + shift_lfo);

        int16_t mixed = +(saw_down      * 127) +
                        -(saw_up        * static_cast<uint8_t>(127 - m_color)) +
                        +(saw_down_copy * m_color);
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_RM_PWM:
      {
        m_phase_0 += (m_freq >> 1);
        m_phase_1 += (m_freq >> 1) + m_freq;
        m_phase_2 += (m_freq << 2) + m_freq;

        result = 0;
        result += (m_color & (1 << 0)) ? get_triangle_wave_level(m_phase_0 << 0) : 0;  // 0.5
        result += (m_color & (1 << 0)) ? get_triangle_wave_level(m_phase_1 << 0) : 0;  // 1.5
        result +=                        get_triangle_wave_level(m_phase_0 << 1);      // 1
        result += (m_color & (1 << 1)) ? get_triangle_wave_level(m_phase_0 << 2) : 0;  // 2
        result += (m_color & (1 << 2)) ? get_triangle_wave_level(m_phase_1 << 1) : 0;  // 3
        result += (m_color & (1 << 3)) ? get_triangle_wave_level(m_phase_0 << 3) : 0;  // 4
        result += (m_color & (1 << 4)) ? get_triangle_wave_level(m_phase_2 << 0) : 0;  // 5
        result += (m_color & (1 << 5)) ? get_triangle_wave_level(m_phase_1 << 2) : 0;  // 6
        result += (m_color & (1 << 6)) ? get_triangle_wave_level(m_phase_0 << 4) : 0;  // 8
        result <<= 5;
      }
      break;
    case OSC_MODE_SAW:
      {
        uint16_t low_freq = value_to_low_freq(m_mod_rate) + 1;
        uint16_t low_freq_x2 = low_freq << 1;
        m_phase_0 += m_freq;
        m_phase_1 += m_freq - low_freq;
        m_phase_2 += m_freq + low_freq;
        m_phase_3 += m_freq - low_freq_x2;
        m_phase_4 += m_freq + low_freq_x2;

        int8_t wave_0 = get_wave_level(m_wave_table, m_phase_0);
        int8_t wave_1 = get_wave_level(m_wave_table, m_phase_1);
        int8_t wave_2 = get_wave_level(m_wave_table, m_phase_2);
        int8_t wave_3 = get_wave_level(m_wave_table, m_phase_3);
        int8_t wave_4 = get_wave_level(m_wave_table, m_phase_4);

        int8_t d = m_mod_depth >> 2;
        int8_t r = (m_color < 64) ? d : -d;
        result = (wave_0 * (126 - d)) + (wave_1 * r)        + (wave_2 * d) +
                                        (wave_3 * (d >> 1)) + (wave_4 * (r >> 1));
      }
      break;
    default:
      {
        m_phase_0 += m_freq;
        result = get_wave_level(g_osc_sine_wave_table_h1, m_phase_0) * 127;
      }
      break;
    }

    return result;
  }

private:
  INLINE static void reset_phase() {
    m_phase_lfo = 0;
    m_phase_0 = 0;
    m_phase_1 = 0;
    m_phase_2 = 0;
    m_phase_3 = 0;
    m_phase_4 = 0;
    m_phase_5 = 0;
  }

  INLINE static uint8_t value_to_low_freq(uint8_t value) {
    return value >> 1;
  }

  INLINE static int8_t mod_rate_to_fm_ratio(uint8_t mod_rate) {
    return (m_mod_rate >> 2) + 1;
  }

  INLINE static int8_t triangle_lfo_clock(uint8_t mod_eg_control, uint8_t mod_rate) {
    m_phase_lfo += value_to_low_freq(mod_rate) + 1;

    uint16_t level = m_phase_lfo;
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte((high_sbyte(level << 1) + 1) * mod_eg_control);
  }

  INLINE static int8_t get_triangle_wave_level(uint16_t phase) {
    uint8_t level = high_byte(phase + 0x4000);
    if ((level & 0x80) != 0) {
      level = ~level;
    }
    level -= 0x40;
    return static_cast<int8_t>(level);
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

template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table;
template <uint8_t T> uint16_t        Osc<T>::m_freq;
template <uint8_t T> uint16_t        Osc<T>::m_phase_lfo;
template <uint8_t T> uint16_t        Osc<T>::m_phase_0;
template <uint8_t T> uint16_t        Osc<T>::m_phase_1;
template <uint8_t T> uint16_t        Osc<T>::m_phase_2;
template <uint8_t T> uint16_t        Osc<T>::m_phase_3;
template <uint8_t T> uint16_t        Osc<T>::m_phase_4;
template <uint8_t T> uint16_t        Osc<T>::m_phase_5;
template <uint8_t T> uint8_t         Osc<T>::m_mode;
template <uint8_t T> uint8_t         Osc<T>::m_color;
template <uint8_t T> uint16_t        Osc<T>::m_mod_rate;
template <uint8_t T> uint16_t        Osc<T>::m_mod_depth;
