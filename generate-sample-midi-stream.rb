require_relative 'DigitalSynthVRA8X/common'

$file = File.open("sample-midi-stream.bin", "wb")

def control_change(control_number, value)
  $file.write([(CONTROL_CHANGE | MIDI_CH), control_number, value].pack("C*"))
end

def play(note_number, length)
  $file.write([(NOTE_ON  | MIDI_CH), note_number, 64].pack("C*"))
  (length * 7 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
  $file.write([(NOTE_OFF | MIDI_CH), note_number, 64].pack("C*"))
  (length * 1 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def wait(length)
  length.times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def play_cegbdfac(c)
  play(12 + (c * 12), 1200)
  play(16 + (c * 12), 1200)
  play(19 + (c * 12), 1200)
  play(23 + (c * 12), 1200)
  play(14 + (c * 12), 1200)
  play(17 + (c * 12), 1200)
  play(21 + (c * 12), 1200)
  play(24 + (c * 12), 6400)
  wait(6400)
end

def sound_off
  control_change(ENV_D        , 0  )
  control_change(ALL_NOTES_OFF, 0  )
  wait(800)
end

sound_off

# Preset Lead
control_change(OSC_MODE     , 0  )
control_change(OSC_COLOR    , 64 )
control_change(MOD_RATE     , 8  )
control_change(MOD_DEPTH    , 64 )
control_change(LPF_CUTOFF   , 127)
control_change(LPF_RESONANCE, 0  )
control_change(ENV_A        , 64 )
control_change(ENV_D        , 64 )
play_cegbdfac(3)

sound_off

$file.close
