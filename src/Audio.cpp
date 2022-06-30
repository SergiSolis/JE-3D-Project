#include "Audio.h"
#include "utils.h"

Audio::Audio() {

}

Audio::~Audio() {

}


void Audio::PlayGameSound(int pos)
{
	//BASS_SetVolume(0.1);

	HSAMPLE hSample;
	hSample = this->samples[pos];
	
	HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);

	std::cout << " + AUDIO PLAYED " << std::endl;
	BASS_ChannelPlay(hSampleChannel, false);
}

void Audio::LoadSample(const char* fileName)
{

	HSAMPLE hSample;

	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, 0);

	if (hSample == 0)
	{
		std::cout << " + Error load  " << fileName << std::endl;
	}
	std::cout << " + AUDIO load " << fileName << std::endl;
	this->samples.push_back(hSample);
}
void Audio::ResetAudio() {

	BASS_Stop();
	BASS_Start();
}