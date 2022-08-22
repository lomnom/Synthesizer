#include <iostream>
#include "Tone.hpp"
#include <algorithm>
#include <math.h>
#include <random>
#include <unordered_map>
#include <time.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#define output std::cerr
#define speaker std::cout
#define shell std::cin
using std::endl;
using std::fill_n;
using std::random_device;
using std::string, std::unordered_map, std::pair,std::thread,std::vector;

unordered_map<counter_t, pair<Wave,volume_t>> tones={};

struct timespec sleep(size_t sec=1,size_t msec=0,size_t mcrsec=0,size_t nsec=0){
	struct timespec tim, tim2;
	tim.tv_sec=sec;
	tim.tv_nsec=nsec+(((msec*1000)+mcrsec)*1000);

	if(nanosleep(&tim , &tim2)<0){
		throw -1;
	}

	return tim2;
}

class Stopwatch{ //hmm
public:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
	double othTime=0;
	bool stopped=false;

	void start(){
		startTime=std::chrono::high_resolution_clock::now();
		stopped=false;
	}

	void stop(){
		std::chrono::duration<double, std::milli> timeTime=std::chrono::high_resolution_clock::now()-startTime;
		othTime+=timeTime.count();
		stopped=true;
	}

	double time(){
		if (!(stopped)){
			std::chrono::duration<double, std::milli> timeTime=std::chrono::high_resolution_clock::now()-startTime;
			return othTime+timeTime.count();
		}else{
			return othTime;
		}
	}

	void reset(){
		stopped=true;
		othTime=0;
	}
};

#define TPS 100
#define TICKLEN (1.0/TPS)
#define TICKFRAMES (RESOLUTION/TPS)
void synth(){
	Stopwatch timekeeper;
	timekeeper.start();
	double properTime=0;

	while (1){
		for (counter_t n=0;n<TICKFRAMES;n++){
			frame_t frame=0;
			for(auto &[id,tone] : tones){
				frame+=tone.first.getFrame() >> tone.second;
			}
			speaker << frame;
		}
		properTime+=TICKLEN*1000;
		// sleep(0,properTime-timekeeper.time());
	}
}

vector<string> tokenize(string s, string del = " ")
{
    int start = 0;
    int end = s.find(del);
    vector<string> tokens;
    while (end != -1) {
        tokens.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    tokens.push_back(s.substr(start, end - start));
    return tokens;
}

bool is_number(const std::string& s){
    return( strspn( s.c_str(), ".0123456789" ) == s.size() );
}

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

	// Syntax:
	//    Tone: 
	//      - T(wave,freq,vol,id)
	//      - T(sine,220,mf,24)
	//      - wave:
	//        - forms: sine,square,triangle,saw
	//      - vol:
	//        - constants: f=0,mf=1,mp=2,p=3,pp=4,ppp=5,pppp=6,ppppp=7
	//      - id:
	//        - value: uint
	//      - freq:
	//        - note: [letter][octave]
	//          - D4n D4b D4#
	//        - freq: uint
	//    DelTone:
	//      - D(id)
	//      - D(24)
	//      - id:
	//        - uint

	thread synthThread (synth);
	// pair<Wave,volume_t> c4={Wave(Pitch(chr2midi('C','4'),440),&sine),mp};
	// pair<Wave,volume_t> F4={Wave(Pitch(chr2midi('F','4'),440),&sine),mp};
	// pair<Wave,volume_t> Bb4={Wave(Pitch(chr2midi('B','4','b'),440),&sine),mp};
	// tones.insert({1,c4});
	// tones.insert({69,F4});
	// tones.insert({420,Bb4});

	while (1){
		string command;
		shell >> command;
		char instruction=command[0];
		vector<string> args=tokenize(command.substr(2, command.size()-3),",");

		if (command[0]=='T'){
			Waveform* form=NULL;
			if (args[0]=="sine"){
				form=&sine;
			} else if (args[0]=="square"){
				form=&square;
			} else if (args[0]=="triangle"){
				form=&triangle;
			} else if (args[0]=="saw"){
				form=&saw;
			}

			Pitch pitch(0);
			if (is_number(args[1])){
				pitch=Pitch(atof(args[1].c_str()));
			} else {
				pitch=Pitch(chr2midi(args[1][0],args[1][1],args[1][2]),440);
			}

			volume_t volume;
			if (args[2]=="f"){
				volume=f;
			} else if (args[2]=="mf"){
				volume=mf;
			} else if (args[2]=="mp"){
				volume=mp;
			} else if (args[2]=="p"){
				volume=p;
			} else if (args[2]=="pp"){
				volume=pp;
			} else if (args[2]=="ppp"){
				volume=ppp;
			} else if (args[2]=="pppp"){
				volume=pppp;
			} else if (args[2]=="ppppp"){
				volume=ppppp;
			} else {
				volume=atoi(args[2].c_str());
			}

			counter_t id=atol(args[3].c_str());

			pair<Wave,volume_t> generated={Wave(pitch,form),volume};
			tones.insert_or_assign(id,generated);
		}else if (command[0]=='D'){
			tones.erase(atol(args[0].c_str()));
		}
	}

	return 0;
}