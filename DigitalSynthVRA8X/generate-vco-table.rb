require_relative 'common'

$file = File.open("vco-table.h", "w")

$file.printf("#pragma once\n\n")

def freq_from_note_number(note_number)
  cent = (note_number * 100.0) - 6900.0
  hz = 440.0 * (2.0 ** (cent / 1200.0))
  freq = (hz * (1 << VCO_PHASE_RESOLUTION_BITS) / SAMPLING_RATE).floor.to_i * 2
end

$file.printf("const uint16_t g_vco_freq_table[] = {\n  ")
((NOTE_NUMBER_MIN - 1)..NOTE_NUMBER_MAX).each do |note_number|
  if (note_number < (NOTE_NUMBER_MIN - 1)) || (note_number > NOTE_NUMBER_MAX)
    freq = 0
  else
    freq = freq_from_note_number(note_number)
  end

  $file.printf("%5d,", freq)
  if note_number == DATA_BYTE_MAX
    $file.printf("\n")
  elsif note_number % 12 == 11
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

$file.printf("const uint16_t g_vco_tune_rate_table[] = {\n  ")
(0..(1 << VCO_TUNE_RATE_TABLE_STEPS_BITS) - 1).each do |i|
  tune_rate = ((2.0 ** (i / (12.0 * (1 << VCO_TUNE_RATE_TABLE_STEPS_BITS)))) *
               (1 << VCO_TUNE_RATE_DENOMINATOR_BITS) / 2.0).floor

  $file.printf("%5d,", tune_rate)
  if i == (1 << VCO_TUNE_RATE_TABLE_STEPS_BITS) - 1
    $file.printf("\n")
  elsif i % 8 == 7
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

$file.close
