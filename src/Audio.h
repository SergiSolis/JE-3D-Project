#ifndef AUDIO_H
#define AUDIO_H
#include "extra/bass.h"
#include <map>
#include <string>
#include <vector>

enum AUDIO_ID : int {
	INTRO_SOUND,
	BACKGROUND_SOUND1,
	BACKGROUND_SOUND2,
	BACKGROUND_SOUND3,
	BACKGROUND_SOUND4,
	BACKGROUND_SOUND5,
	SWORD_SOUND,
	ARROW_SOUND,
	HIT_SOUND,
	HIT_ENEMY_SOUND,
	DEAD_SOUND,
	DEAD_ENEMY_SOUND,
	JUMP,
	TOUCH_GROUND,
	CHEST
};

class Audio
{
public:
	std::vector<HSAMPLE> samples; 
	HSAMPLE sample; 

	Audio(); 
	~Audio(); 

	//initial function se usa de prueba
	void PlayGameSound(int pos);
	void LoadSample(const char* fileName);
	void ResetAudio();
	
};

#endif