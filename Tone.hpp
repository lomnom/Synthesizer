#include <math.h>

// ALL THESE CAN BE CHANGED BUT MIGHT BREAK STUFF

//data type for every audio frame. should always be an unsigned int type
typedef uint8_t frame_t;

//define type for misc counters
typedef uint32_t counter_t; 

//define type for precise counters
typedef long double pcounter_t;

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

//half of the max, rounded down
#define HALFMAX WAVEMAX/2

//rate of audio
//#define RATE 11025
#define RATE 22050

//size of buffer for a wave phase. should be RATE/60
//#define RESOLUTION 184 
#define RESOLUTION 368

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

midi_t chr2midi(char note,char octave,char sharpflat){
	octave-=48;
	int8_t offset= sharpflat=='n' ? 0 : (sharpflat=='s' ? 1 : -1);
	return 21+(12*octave)+midiMap[note-65]+offset;
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

struct Wave{
	Waveform* form; //form of the wave
	Pitch pitch;    //pitch of the wave
	pcounter_t lutP=0; //position in wave 
	pcounter_t framespread; //steps in LUT, frames per phase
	counter_t frame=0;
	bool inverted=false;

	Wave(Pitch pitch,Waveform* form): form(form), pitch(pitch){
		framespread=float(RESOLUTION)/((float)RATE/pitch.freq)/(2-form->flips);
	}

	frame_t getFrame(){
		frame_t rawFrame=form->phase[int(lutP)];

		if (form->flips){
			rawFrame=rawFrame>>1;
			if (inverted) 
				rawFrame+=HALFMAX;
			else
				rawFrame=HALFMAX-rawFrame;
		}
		
		lutP+=framespread;
		frame++;
		if (lutP>RESOLUTION){
			lutP-=RESOLUTION;
			inverted=!inverted;
		}

		return rawFrame;
	}
};


enum volume: volume_t {
	f=0,
	mf=1,
	mp=2,
	p=3,
	pp=4,
	ppp=5,
	pppp=6,
	ppppp=7
};

