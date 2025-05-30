﻿#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H
#define adap AutoDeleteAudioPlayer::getInstance()

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <utility>

class AudioPlayer
{
public:
	explicit AudioPlayer(const std::string& filename);
	~AudioPlayer();

	void play(int loopCount = 1);
	void stop();
	void setLoopCount(int count);
	void setVolume(float volume);

	bool isPlaying() const;

private:
	static ALCdevice* m_device;
	static ALCcontext* m_context;
	static std::atomic<int> m_instanceCount;

	ALuint m_source;
	ALuint m_buffer;
	std::thread m_playbackThread;
	std::mutex m_mutex;
	std::atomic<bool> m_isPlaying;
	int m_loopCount;

	bool loadWavFile(const std::string& filename,
		ALenum& format,
		ALvoid*& data,
		ALsizei& size,
		ALsizei& freq);
	void playbackLoop();
};

class AutoDeleteAudioPlayer
{
public:
	AutoDeleteAudioPlayer();
	static AutoDeleteAudioPlayer* getInstance();
	void addAudio(std::string filename, float volume = 1.0f);
private:
	std::atomic<bool> mBeingDeleted;
	std::vector<AudioPlayer*> mAudioPlayerList;
	std::thread mThread;
	void autoCheckLoop();

	static AutoDeleteAudioPlayer* instance;
	~AutoDeleteAudioPlayer();
};
#endif