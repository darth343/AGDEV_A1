#ifndef SOUND_H
#define SOUND_H

#include "irrKlang.h"
#include <string>
using std::string;

#pragma comment(lib, "irrKlang.lib")

class Sound
{
public:

	Sound();
	~Sound();
	void playMusic(string Music);
	void stopMusic(string Music);
	void setIsPaused(bool set);
	void dropMusic();
	void playSE(string Music, bool startpaused = false);
	bool getPaused();
	void play3dSound(string Music, irrklang::vec3df pos, irrklang::vec3df view, irrklang::vec3df targetpos);
	irrklang::ISound* getSound();
	void setSound(irrklang::ISound* sound);
	void pause();
	
private:
	irrklang::ISoundEngine* musicEngine;
	irrklang::ISoundEngine* SoundEffect3D;
	irrklang::ISound* Song;
	irrklang::ISound* SoundEffect;
};
#endif