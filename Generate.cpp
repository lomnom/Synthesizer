#include <iostream>
#include <algorithm>
#include "Tone.hpp"
#include <math.h>
#define output std::cerr
#define speaker std::cout
using std::endl;
using std::fill_n;

int main(int argc, char **argv){
	//define waveforms
	frame_t squareLUT[RESOLUTION]={0};
	fill_n(squareLUT, RESOLUTION, WAVEMAX);
	Waveform square=Waveform(squareLUT,true);

	frame_t sawLUT[RESOLUTION]={0};
	float frameStep=float(WAVEMAX)/RESOLUTION;
	counter_t firstHalfSz=RESOLUTION/2;
	counter_t secHalfSz=RESOLUTION-firstHalfSz;
	for (uint frame=0;frame<firstHalfSz;frame++){
		sawLUT[frame]=frameStep*(frame+secHalfSz);
	}
	for (uint frame=0;frame<secHalfSz;frame++){
		sawLUT[frame+firstHalfSz]=frameStep*frame;
	}
	Waveform saw=Waveform(sawLUT,false);

	frame_t sineLUT[RESOLUTION]={0};
	double pi = 2*asin(1.0);
	for (uint frame=0;frame<RESOLUTION;frame++){
		sineLUT[frame]=WAVEMAX*sin(pi*(frame/753.0));
	}
	Waveform sine=Waveform(sineLUT,true);

	frame_t triangleLUT[RESOLUTION]={0};
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
		Wave tone=Wave(pitch,&triangle);
		output << tone.realfreq << " <(realfreq) "
			   << Pitch(tone.freq).centsFrom(tone.realfreq) << " <(centsaway)";

		//display debug information on waveforms
		for (uint p=0; p<tone.frames; p++){ //print information on wave lookup table
			output << tone.fLUT[p] << " <(LUT) " << p << " <(index) " 
			       << (uint)tone.form->phase[tone.fLUT[p]] << " <(value)" << endl;
		}
		output << "ENDLUT" << endl;

		//display information on resulting waveform
		for (uint n=0; n<50; n++)
			output << tone.wavepos << " <(wave) " 
			       << tone.onsecondphase << " <(sp) " << (uint)tone.getFrame() 
			       << " <(frame) " << endl;

		output << "Generating 15 seconds of waveform..." << endl;

		//send wave to stdout
		for (uint n=0; n<RATE*15;n++){
			speaker << tone.getFrame();
		}
	#endif

	#ifdef chromatic
		Waveform* tone=&triangle;
		midi_t freq=chr2midi('C','2');
		midi_t end=chr2midi('B','6');
		for (;freq!=end;freq++){
			Wave tone=Wave(Pitch(freq,440),&triangle);
			output << int(freq) << " <(pitch) " << tone.freq << " <(freq) " 
			       << tone.realfreq << " <(realfreq) "
			       << Pitch(tone.freq).centsFrom(tone.realfreq) << " <(centsaway)" << endl;
			for (uint n=0; n<RATE/4;n++){
				frame_t frame=tone.getFrame() >> mf;
				speaker << frame;
			}
		}
	#endif

	#define chords
	#ifdef chords
		Wave* tone[argc-1];
		for (int n=1;n<argc;n++){
			tone[n-1]=new Wave(Pitch(chr2midi(argv[n][0],argv[n][1],argv[n][2]),440),&triangle);
			output << (int)tone[n-1]->pitch.midi << " <(midi) "
			       << tone[n-1]->freq << " <(freq) ";

			output << tone[n-1]->realfreq << " <(realfreq) "
			       << Pitch(tone[n-1]->freq).centsFrom(tone[n-1]->realfreq) << " <(centsaway)" << endl;
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