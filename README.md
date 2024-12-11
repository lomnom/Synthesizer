# Synthesizer
A synthesizer for basic waveforms (triangle, saw, sine) which supports practically infinite polyphony. Kinda janky and has latency due to out321 & pipe buffering.
## Installation (mac/linux only) 
1. Install mpg321 (for out123)
2. Pull this repo

## Usage
1. Run `Synthesizer.bash`
2. Into stdin, create/delete tones with the following syntax, seperated by a newline each
```
 //    Each tone has a unique id.
	//    Tone: To create or alter a tone
	//      - T(wave,freq,vol,id) yes, no space
	//      - Example: T(sine,220,mf,24)
	//      - wave:
	//        - forms: sine,square,triangle,saw
	//      - vol:
	//        - constants: f=0,mf=1,mp=2,p=3,pp=4,ppp=5,pppp=6,ppppp=7
	//      - id:
	//        - value: uint
	//      - freq:
	//        - note: [letter][octave][accidental]
	//          - D4n D4b D4#
	//        - freq: uint
 //          - 220
	//    DelTone:
	//      - D(id)
	//      - D(24)
	//      - id:
	//        - uint
```
