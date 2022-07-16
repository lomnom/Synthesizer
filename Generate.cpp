#include <iostream>
#include <algorithm>
#include "Tone.hpp"
#include <math.h>
#define output std::cerr
#define speaker std::cout
using std::endl;
using std::fill_n;

int main(){
	//define pitch to synthesize
	Pitch pitch=Pitch(chr2midi('D','4'),440);
	output << pitch.freq << "freq\n";

	//define waveforms
	frame_t squareLUT[RESOLUTION]={0};
	fill_n(squareLUT, RESOLUTION, 255);
	Waveform square=Waveform(squareLUT,true);

	frame_t sawLUT[RESOLUTION]={0};
	float frameStep=255.0/RESOLUTION;
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
		sineLUT[frame]=255*sin(pi*(frame/753.0));
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

	//define tone to synthesize
	Wave tone=Wave(pitch,&triangle);

	//display debug information on waveforms
	for (uint p=0; p<tone.frames; p++){ //print information on wave lookup table
		output << tone.fLUT[p] << " <(LUT) " << p << " <(index) " 
		       << (uint)tone.form->phase[tone.fLUT[p]] << " <(value)" << endl;
	}
	output << "ENDLUT" << endl;

	//display information on resulting waveform
	for (uint n=0; n<50; n++)
		output << tone.wavepos << " <(wave) " 
		       << tone.onsecondphase << " <(sp) " << (uint)tone.getFrame() << " <(frame) " << endl;

	output << "Generating 15 seconds of waveform..." << endl;

	//send wave to stdout
	for (uint n=0; n<RATE*15;n++){
		speaker << tone.getFrame();
	}

	return 0;
}