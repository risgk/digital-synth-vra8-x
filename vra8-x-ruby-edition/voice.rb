require_relative 'common'

class Voice
  def initialize
    @vco = IVCO.new
    @vca = IVCA.new
    @eg  = IEG.new
    @note_number = NOTE_NUMBER_MIN
  end

  def note_on(note_number)
    if ((note_number >= NOTE_NUMBER_MIN) && (note_number <= NOTE_NUMBER_MAX))
      @note_number = note_number
      @eg.note_on
    end
  end

  def note_off
    @eg.note_off
  end

  def control_change(controller_number, controller_value)
    case (controller_number)
    when OSC_TYPE
      # TODO
    when OSC_COLOR
      # TODO
    when MOD_FREQ
      # TODO
    when MOD_DEPTH
      # TODO
    when ENV_A
      @eg.set_attack(controller_value)
    when ENV_D
      @eg.set_decay_release(controller_value)
    when ENV_S
      @eg.set_sustain(controller_value)
    when ENV_R
      # TODO
    when ALL_NOTES_OFF
      @eg.note_off
    end
  end

  def clock
    eg_output = @eg.clock
    vco_output = @vco.clock(@note_number << 8, eg_output, 0)
    vca_output = @vca.clock(vco_output, eg_output)
    return high_sbyte(vca_output)
  end
end
