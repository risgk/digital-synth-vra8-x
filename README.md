# Digital Synth VRA8-X 0.0.0

- 2015-11-** ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-x>

## Concept

- Experimental Synthesizer (MIDI Sound Module) for Arduino Uno

## Features

- Sketch for Arduino Uno
- Serial MIDI In (38400 bps), PWM Audio Out (Pin 6), PWM Rate: 62500 Hz
- Sampling Rate: 15625 Hz, Bit Depth: 8 bits
- LPF Attenuation Slope: -12 dB/oct
- Recommending [Hairless MIDI<->Serial Bridge](http://projectgus.github.io/hairless-midiserial/) to connect PC
- Files
    - `DigitalSynthVRA8X/DigitalSynthVRA8X.ino` is for Arduino Uno
    - `MakeSampleWavFile.cc` is for Debugging on PC, `make-sample-wav-file-cc.bat` makes a sample WAV file
        - Requiring GCC (G++) or other

## VRA8-X CTRL

- Parameter Editor (MIDI Controller) for VRA8-X, Web App
- We recommend Google Chrome, which implements Web MIDI API
- Recommending [loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html) (virtual loopback MIDI cable) to connect VRA8-X
- CAUTION: Click sounds occur sometimes when you change the controllers

## Controllers

### Oscillator's

    +----------+---------+-------------------+---------------+-----------------+------------------+
    | OSC MODE | Name    | Explanation       | OSC COLOR     | MOD RATE        | MOD DEPTH        |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 0        | SAW     | Saw Wave(s)       | Waveform      | Detune          | Detune Mix       |
    |          |         |                   | Selection     | 0.48 ~ 30.7 Hz  | 0% ~ 100%        |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 16       | ----    | (Reserved)        | ----          | ----            | ----             |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 32       | RM+PWM  | Ring Modulation + | ????          | PWM Rate        | PWM Depth        |
    |          |         | PWM               |               | 0.24 ~ 15.4 Hz  | 0% ~ 100%        |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 48       | ----    | (Reserved)        | ----          | ----            | ----             |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 64       | PWM+SAW | Pulse + Saw       | Waveform Mix  | PWM Rate        | PWM Depth        |
    |          |         |                   | Pulse 100% ~  | 0.24 ~ 15.4 Hz  | 0% ~ 100%        |
    |          |         |                   | Saw 100%      |                 |                  |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 80       | ----    | (Reserved)        | ----          | ----            | ----             |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 96       | SYNC+FM | Oscillator Sync + | Detune        | Frequency Ratio | Modulation Index |
    |          |         | FM Synth          | 0 ~ 15.1 Hz   | 0.500 ~ 16.375  | 0 ~ 12.6         |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 112      | ----    | (Reserved)        | ----          | ----            | ----             |
    +----------+---------+-------------------+---------------+-----------------+------------------+
    | 127      | FM      | FM Synth          | Detune        | Frequency Ratio | Modulation Index |
    |          |         |                   | 0 ~ 15.1 Hz   | 0.5 ~ 16.0      | 0 ~ 12.6         |
    +----------+---------+-------------------+---------------+-----------------+------------------+

### Others

    +----------------+-----------------+----------------+-----------------+---------------+
    | Controller     | Value 0         | Value 64       | Value 127       | Notes         |
    +----------------+-----------------+----------------+-----------------+---------------+
    | LPF CUTOFF/ENV | 0.22 kHz ~      | 0.22 kHz ~     | 7.0 kHz ~       | Envelope 0% ~ |
    |                | 0.22 kHz        | 7.0 kHz        | 7.0 kHz         | Envelope 100% |
    +----------------+-----------------+----------------+-----------------+---------------+
    | LPF RESONANCE  | Q = 1.0         | Q = 2.0        | Q = 4.0         |               |
    +----------------+-----------------+----------------+-----------------+---------------+
    | ENV A          | 4.2 ms          | 0.14 s         | 4.2 s           |               |
    +----------------+-----------------+----------------+-----------------+---------------+
    | ENV D/R        | Decay 100.0 ms  | Decay 100.0 s  | Decay 10.0 s    |               |
    |                | Release 10.0 ms | Release 10.0 s | Release 10.0 ms |               |
    +----------------+-----------------+----------------+-----------------+---------------+

## MIDI Implementation Chart

      [Experimental Synthesizer]                                      Date: 2015-11-**       
      Model  Digital Synth VRA8-X     MIDI Implementation Chart       Version: 0.0.0         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | Mode 4 (M=1)  |                       |
    |              Messages         | x             | x             |                       |
    |              Altered          | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Note                          | x             | 0-127         |                       |
    | Number       : True Voice     | ************* | 24-84         |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Velocity     Note ON          | x             | x             |                       |
    |              Note OFF         | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | After        Key's            | x             | x             |                       |
    | Touch        Ch's             | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Pitch Bend                    | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Control                    16 | x             | o             | OSC MODE              |
    | Change                     17 | x             | o             | OSC COLOR             |
    |                            18 | x             | o             | MOD RATE              |
    |                            19 | x             | o             | MOD DEPTH             |
    |                            20 | x             | o             | LPF CUTOFF/ENV        |
    |                            21 | x             | o             | LPF RESONANCE         |
    |                            22 | x             | o             | ENV A                 |
    |                            23 | x             | o             | ENV D/R               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Program                       | x             | x             |                       |
    | Change       : True #         | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System Exclusive              | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Song Pos       | x             | x             |                       |
    | Common       : Song Sel       | x             | x             |                       |
    |              : Tune           | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Clock          | x             | x             |                       |
    | Real Time    : Commands       | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Aux          : Local ON/OFF   | x             | x             |                       |
    | Messages     : All Notes OFF  | x             | o             |                       |
    |              : Active Sense   | x             | x             |                       |
    |              : Reset          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Notes                         |                                                       |
    |                               |                                                       |
    +-------------------------------+-------------------------------------------------------+
      Mode 1: Omni On,  Poly          Mode 2: Omni On,  Mono          o: Yes                 
      Mode 3: Omni Off, Poly          Mode 4: Omni Off, Mono          x: No                  
