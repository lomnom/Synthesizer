#include <math.h>

// ALL THESE CAN BE CHANGED BUT CHANGING MIDI MIGHT BREAK STUFF

//data type for every audio frame
typedef uint8_t frame_t;

//define type for misc counters
typedef uint32_t counter_t; 

//define type for midi notes, Do not change this.
typedef uint8_t midi_t;

//define type for storing Hertz
typedef float hertz_t;

//define type for storing cents
typedef float cents_t;

//define type for storing volume
typedef uint8_t volume_t;

//maximum value of every audio frame
#define WAVEMAX std::numeric_limits<frame_t>::max()

//rate of audio
#define RATE 11025

//minumum frequency for a clear non-stretched wave
#define CLARITYTHRESH 60 

//size of buffer for a wave phase. should be RATE/60
#define RESOLUTION 184 

struct Waveform{
	frame_t* phase; //phase buffer
	bool flips; //plays inverted every other iteration?
	//phase: pointer to start of RESULUTION sized phase buffer
	//flips: does the phase play inverted every second iteration
	Waveform(frame_t* phase, bool flips): phase(phase), flips(flips){
	}
};

int8_t midiMap[7]={0,2,-9,-7,-5,-4,-2};
midi_t chr2midi(char note,char octave){
	octave-=48;
	return 21+(12*octave)+midiMap[note-65];
}

struct Pitch{
	hertz_t freq; //frequency in hertz
	midi_t midi; //midi

	Pitch(hertz_t freq): freq(freq), midi(0){
	}

	Pitch(midi_t midi,hertz_t a): midi(midi){ //a is usually 440
		freq= (a / 32) * pow(2, ((midi - 9) / 12.0)); //midi to hertz taken from 
		//https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
	}

	cents_t centsFrom(Pitch pitch){ //calculate difference in cents.
		return 1200 * log2f( pitch.freq / this->freq ); //algorithm for cent interval, wikipedia
	}

	Pitch addCents(cents_t cents) { //add cents to current pitch
		return Pitch( freq*2*(cents/1200) ); //also from wikipedia
	}
};

struct Wave{ //slightly off, but performance > super accurate
	Waveform* form; //form of the wave
	Pitch pitch;    //pitch of the wave
	counter_t wavepos=0; //position in wave phase
	int16_t* fLUT; //frame position lookup table
	bool onsecondphase=false; //is it on an even phase?
	counter_t frames; //number of frames per phase
	float freq,realfreq; //proper frequency and real frequency

	counter_t phasepos, iteration;

	Wave(Pitch pitch,Waveform* form): form(form), pitch(pitch){
		frames=RATE/pitch.freq;
		realfreq=RATE/frames;
		freq=pitch.freq;
		fLUT=new int16_t[frames];
		float framespread=float(RESOLUTION)/frames;
		for (counter_t frame=0; frame<frames; frame++){
			fLUT[frame]=framespread*frame;
		}
	}

	~Wave(){
		delete[] fLUT;
	}

	frame_t getFrame(){
		frame_t rawFrame;

		if (form->flips){
			rawFrame=form->phase[fLUT[wavepos]]>>1;
			if (onsecondphase) 
				rawFrame+=128;
			else
				rawFrame=127-rawFrame;
		} else {
			rawFrame=form->phase[fLUT[wavepos]];
		}
		wavepos+=1;

		if (wavepos==frames){
			wavepos=0;
			onsecondphase=!onsecondphase;
		}


		return rawFrame;
	}
};
