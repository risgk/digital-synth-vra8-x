#pragma once

// refs http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class Filter {
  static int16_t        m_x_1;
  static int16_t        m_x_2;
  static int16_t        m_y_1;
  static int16_t        m_y_2;
  static uint16_t       m_cutoff;
  static uint8_t        m_cutoff_env_amt;
  static uint8_t        m_resonance;

  static const uint8_t AUDIO_FRACTION_BITS = 14;

public:
  INLINE static void initialize() {
    m_x_1 = 0;
    m_x_2 = 0;
    m_y_1 = 0;
    m_y_2 = 0;
    set_cutoff(127);
    set_resonance(0);
    set_cutoff_env_amt(0);
  }

  INLINE static void set_cutoff(uint8_t controller_value) {
    m_cutoff = controller_value;
  }

  INLINE static void set_resonance(uint8_t controller_value) {
    m_resonance = controller_value;
  }

  INLINE static void set_cutoff_env_amt(uint8_t controller_value) {
    m_cutoff_env_amt = controller_value;
  }

  INLINE static int16_t clock(int16_t audio_input, uint8_t cutoff_env_control) {
    uint8_t cutoff = m_cutoff + high_byte(m_cutoff_env_amt * cutoff_env_control);
    if (cutoff > 127) {
      cutoff = 127;
    }

    // todo
    return audio_input;
  }
};

template <uint8_t T> int16_t        Filter<T>::m_x_1;
template <uint8_t T> int16_t        Filter<T>::m_x_2;
template <uint8_t T> int16_t        Filter<T>::m_y_1;
template <uint8_t T> int16_t        Filter<T>::m_y_2;
template <uint8_t T> uint16_t       Filter<T>::m_cutoff;
template <uint8_t T> uint8_t        Filter<T>::m_cutoff_env_amt;
template <uint8_t T> uint8_t        Filter<T>::m_resonance;
