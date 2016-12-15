#include "Sound.h"

Sound::Sound()
{
	musicEngine = irrklang::createIrrKlangDevice();
	SoundEffect3D = irrklang::createIrrKlangDevice();
	SoundEffect3D->setDefault3DSoundMinDistance(1.f);
	SoundEffect3D->setDefault3DSoundMaxDistance(10.f);
	SoundEffect = NULL;
}
Sound::~Sound()
{
	musicEngine->drop();
	//SoundEffect->drop();
	SoundEffect3D->drop();
}

void Sound::playMusic(string Music)
{
	//Song->setVolume(0.5f);
	Song = musicEngine->play2D(Music.c_str(), true, false, true);
}

void Sound::playSE(string Music, bool startpaused)
{
	SoundEffect = musicEngine->play2D(Music.c_str(), false, startpaused, false);
}
void Sound::play3dSound(string Music, irrklang::vec3df pos, irrklang::vec3df view, irrklang::vec3df targetpos)
{
	//SoundEffect3D->setSoundVolume(2.0f);
	SoundEffect3D->play3D(Music.c_str(), targetpos);
	SoundEffect3D->setListenerPosition(pos, view);
}

void Sound::stopMusic(string Music)
{
	Song->stop();
}
void Sound::setIsPaused(bool set)
{
	SoundEffect->setIsPaused(set);
}

void Sound::pause()
{
	Song->setIsPaused(true);
}

bool Sound::getPaused()
{
	return SoundEffect->getIsPaused();
}
void Sound::dropMusic()
{
	Song->drop();
}
irrklang::ISound* Sound::getSound()
{
	return SoundEffect;
}
void Sound::setSound(irrklang::ISound* sound)
{
	this->SoundEffect = sound;
}