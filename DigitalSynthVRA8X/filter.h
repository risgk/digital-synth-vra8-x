#pragma once

// refs http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

#include "common.h"
#include "mul-q.h"
#include "filter-table.h"

template <uint8_t T>
class Filter {
  static int8_t         m_y_1;
  static int8_t         m_y_2;
  static uint16_t       m_cutoff;
  static const uint8_t* m_lpf_table;
  static uint8_t        m_cutoff_env_amt;

  static const uint8_t AUDIO_FRACTION_BITS = 14;

public:
  INLINE static void initialize() {
    m_y_1 = 0;
    m_y_2 = 0;
    set_cutoff(127);
    set_resonance(0);
  }

  INLINE static void set_cutoff(uint8_t controller_value) {
    m_cutoff = controller_value;
    m_cutoff_env_amt = 0;
  }

  INLINE static void set_resonance(uint8_t controller_value) {
    if (controller_value >= 96) {
      m_lpf_table = g_filter_lpf_table_q_4_sqrt_2;
    } else if (controller_value >= 32) {
      m_lpf_table = g_filter_lpf_table_q_2;
    } else {
      m_lpf_table = g_filter_lpf_table_q_1_over_sqrt_2;
    }
  }

  INLINE static int16_t clock(int16_t audio_input, uint8_t cutoff_env_control) {
    uint8_t cutoff = m_cutoff + high_byte(m_cutoff_env_amt * cutoff_env_control);
    if (cutoff > 127) {
      cutoff = 127;
    }

    const uint8_t* p = m_lpf_table + (cutoff * 3);
    uint8_t  b_2_over_a_0_low  = *p++;
    uint8_t  b_2_over_a_0_high = *p++;
    uint8_t  a_1_over_a_0_high = *p;
    uint16_t b_2_over_a_0      = b_2_over_a_0_low | (b_2_over_a_0_high << 8);
    uint8_t  a_2_over_a_0_high = b_2_over_a_0_high + a_1_over_a_0_high - 128;

    int8_t x_0   = high_byte(audio_input);
    int16_t tmp  = mul_q16_q7(b_2_over_a_0, x_0);
    tmp         += m_y_1 * a_1_over_a_0_high;
    tmp         -= m_y_2 * a_2_over_a_0_high;
    if (tmp > 127*256) {
      tmp = 127*256;
    }
    if (tmp < -128*256) {
      tmp = -128*256;
    }
    int8_t y_0 = (tmp << 1) >> 8;

    m_y_2 = m_y_1;
    m_y_1 = y_0;

    return y_0 << 8;
  }
};

template <uint8_t T> int8_t         Filter<T>::m_y_1;
template <uint8_t T> int8_t         Filter<T>::m_y_2;
template <uint8_t T> uint16_t       Filter<T>::m_cutoff;
template <uint8_t T> const uint8_t* Filter<T>::m_lpf_table;
template <uint8_t T> uint8_t        Filter<T>::m_cutoff_env_amt;
