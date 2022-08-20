#include <iostream>
#include "Tone.hpp"
#include <algorithm>
#include <math.h>
#include <random>
#define output std::cerr
#define speaker std::cout
using std::endl;
using std::fill_n;
using std::random_device;

int main(int argc, char **argv){
	//define waveforms
	frame_t squareLUT[RESOLUTION]={0};
	fill_n(squareLUT, RESOLUTION, WAVEMAX);
	Waveform square=Waveform(squareLUT,true);

	frame_t sawLUT[RESOLUTION]={0};
	float frameStep=float(WAVEMAX)/RESOLUTION;
	for (uint frame=RESOLUTION;frame!=0;frame--){
		sawLUT[frame]=frameStep*frame;
	}
	Waveform saw=Waveform(sawLUT,false);

	frame_t sineLUT[RESOLUTION]={0};
	double pi = 2*asin(1.0);
	for (uint frame=0;frame<RESOLUTION;frame++){
		sineLUT[frame]=WAVEMAX*sin(pi*((float)frame/RESOLUTION));
	}
	Waveform sine=Waveform(sineLUT,true);

	frame_t triangleLUT[RESOLUTION]={0};
	counter_t firstHalfSz=HALFMAX;
	counter_t secHalfSz=RESOLUTION-firstHalfSz;
	for (uint frame=0;frame<firstHalfSz;frame++){
		triangleLUT[frame]=2*frame_t(frameStep*(frame));
	}
	for (uint frame=0;frame<secHalfSz;frame++){
		triangleLUT[frame+firstHalfSz]=2*frame_t(frameStep*(secHalfSz-frame));
	}
	Waveform triangle=Waveform(triangleLUT,true);

	#ifdef singlepitch
		//define pitch to synthesize
		Pitch pitch=Pitch(chr2midi('D','4'),440);
		output << pitch.freq << " <(freq)\n";

		//define tone to synthesize
		Wave tone=Wave(pitch,&saw);
		output << tone.pitch.freq << endl;

		//display information on resulting waveform
		for (uint n=0; n<50; n++)
			output << tone.frame << " <(frameN) " 
			       << tone.lutP << " <(lutP) " << (uint)tone.getFrame() 
			       << " <(frame) "
			       << (tone.inverted ? "True" : "False") << " <(inverted?)"  << endl;

		output << "Generating 15 seconds of waveform..." << endl;

		//send wave to stdout
		for (uint n=0; n<RATE*15;n++){
			frame_t frame=tone.getFrame() >> mf;
			speaker << frame;
		}
	#endif

	#ifdef chromatic
		Waveform* tone=&triangle;
		midi_t freq=chr2midi('C','2');
		midi_t end=chr2midi('B','6');
		frame_t frame;
		for (;freq!=end;freq++){
			Wave tone=Wave(Pitch(freq,440),&triangle);
			output << int(freq) << " <(pitch) " << tone.pitch.freq << " <(freq) " << endl;
			for (uint n=0; n<RATE;n++){
				frame=tone.getFrame() >> mf;
				speaker << frame;
			}
			while (frame!=(HALFMAX>>mf)){
				frame+=frame>(HALFMAX>>mf) ? -1 : 1;
				speaker << frame;
			}
		}
	#endif

	#define chords
	#ifdef chords
		Wave* tone[argc-1];
		for (int n=1;n<argc;n++){
			tone[n-1]=new Wave(Pitch(chr2midi(argv[n][0],argv[n][1],argv[n][2]),440),&triangle);

			output << argv[n] << ": " << tone[n-1]->pitch.freq << " <(freq) " << 
			          (int)tone[n-1]->pitch.midi << " <(midi) " << endl;
		}

		for (uint n=0; n<RATE*15;n++){
			frame_t frame=0;
			for (uint t=0;t<argc-1;t++){
				frame+=tone[t]->getFrame() >> p;
			}
			speaker << frame;
		}
	#endif

	return 0;
}