# papesch

## Instalation

You have to have libfftw3-dev and libsndfile1-dev installed on your computer.
Then you simply use make to compile the program. The binary will be located in bin/audio-filter.

I use the c++17 `<filesystem>` header, which I had to use via the experimental namespace, this may very
on your computer. If this is a problem, remove every instance of `experimental` and any non-alphanumeric characters after
in `main.cpp`.

## Usage

After you compiled the program, it is ran from the command line as such:

`audio-filter -i <input file path> -o <output file path> -c <config file path> [-w <window size>]`

`<input file path>` - path to an existing audio file (tested on wav format)

`<output file path>` - path to the place where the filtered audio should be stored

`<config file path>` - path to the config file

`<window size>` - the size of the window to be used for effects, powers of 2 are reccomended, this argument is optional with a default value set to 8192.

Some sample sounds and configurations are prepared in `demo-sounds` and `demo-conf` directories.
If not said otherwise, the present sounds have been created by the autor of this program (Jan Papesch).

The exceptions are:

- guitar-riff.wav (taken from [here](https://freesound.org/people/pitx/sounds/16063/))

### Config file

Each line of the config file stands for one filter to be used on the source audio.

The filters are applied in the order that they are written in the file.

Format of each line is: `<filter_name> [arguments...]` where there can be none or more arguments.
Lines starting with `#` are skipped.

Valid filter names and arguments:

- `lowpass <cutoff>`
  - cutoff specifies what range of frequencies are cutoff, must be a double in range [0,1].
- `highpass <cutoff>`
  - cutoff specifies what range of frequencies are cutoff, must be a double in range [0,1].
- `delay <mode> <additional args>`
  - `auto` mode
    - `delay auto <type> <delay> <count> [base]`
      - `type` linear or exp
      - `delay` time between delayed repetetions (in ms)
      - `count` number of delayed repetetions
      - `base` used for `exp` type to set the fadeout speed
  - `manual` mode
    - `delay manual <delay 1> <gain 1> <delay 2> <gain 2> ... <delay n> <gain n>`
      - full control of delays (in ms) and gains for each "voice"
- `pitchshift <ammount>`
  - ammount specifies how much will the pitch be shifted, must be a non-negative double. Values > 1 increase the pitch and < 1 decrease. Ammount 2 corresponds to a shift 1 octave up, 0.5 1 octave down.
- `chorus <mode> <voice count> <max delay> <LFO frequency base> <LFO frequency variance> <LFO amplitude base> <LFO amplitude variance> <pitch variance> <gain>`
  - `mode`: `random` or `uniform`
  - `voice count` the number of the voices to add
  - `max delay` maximal delay (in microseconds)
  - `LFO *` the argument for the pitch shift Low Frequency Oscilator
    - frequency - in Hz
    - `LFO amplitude` 1...pitch shift ranges +-1 octave, 2..ranging +-2 octaves 1/12=0.083333...+- 1 semitone
  - `pitch variance` the variance of the midpoint of the LFO
  - `gain` the gain of the added voices
