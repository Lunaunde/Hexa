#include "AudioPlayer.h"
#include <iostream>
#include <fstream>
#include <cstdint>

ALCdevice* AudioPlayer::m_device = nullptr;
ALCcontext* AudioPlayer::m_context = nullptr;
std::atomic<int> AudioPlayer::m_instanceCount(0);

AudioPlayer::AudioPlayer(const std::string& filename)
    : m_isPlaying(false), m_loopCount(1) {

    m_instanceCount++;
    if (m_instanceCount == 1) {
        m_device = alcOpenDevice(nullptr);
        if (!m_device) {
            std::cerr << "Failed to open OpenAL device" << std::endl;
            return;
        }
        m_context = alcCreateContext(m_device, nullptr);
        if (!m_context || !alcMakeContextCurrent(m_context)) {
            std::cerr << "Failed to create OpenAL context" << std::endl;
            return;
        }
    }

    ALenum format;
    ALvoid* data;
    ALsizei size, freq;
    if (!loadWavFile(filename, format, data, size, freq)) {
        std::cerr << "Failed to load WAV file: " << filename << std::endl;
        return;
    }

    alGenBuffers(1, &m_buffer);
    alBufferData(m_buffer, format, data, size, freq);
    delete[] static_cast<char*>(data);

    alGenSources(1, &m_source);
    alSourcei(m_source, AL_BUFFER, m_buffer);
}

AudioPlayer::~AudioPlayer() {
    stop();
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);

    m_instanceCount--;
    if (m_instanceCount == 0) {
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        m_context = nullptr;
        m_device = nullptr;
    }
}

bool AudioPlayer::loadWavFile(const std::string& filename,
    ALenum& format,
    ALvoid*& data,
    ALsizei& size,
    ALsizei& freq) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Read WAV header
    char chunkID[4];
    file.read(chunkID, 4);
    if (strncmp(chunkID, "RIFF", 4) != 0) {
        std::cerr << "Not a valid WAV file" << std::endl;
        return false;
    }

    uint32_t chunkSize;
    file.read(reinterpret_cast<char*>(&chunkSize), 4);

    char formatID[4];
    file.read(formatID, 4);
    if (strncmp(formatID, "WAVE", 4) != 0) {
        std::cerr << "Not a valid WAV file" << std::endl;
        return false;
    }

    // Read fmt subchunk
    char subchunk1ID[4];
    file.read(subchunk1ID, 4);
    if (strncmp(subchunk1ID, "fmt ", 4) != 0) {
        std::cerr << "fmt subchunk not found" << std::endl;
        return false;
    }

    uint32_t subchunk1Size;
    file.read(reinterpret_cast<char*>(&subchunk1Size), 4);
    uint16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), 2);
    if (audioFormat != 1) {
        std::cerr << "Unsupported audio format (only PCM supported)" << std::endl;
        return false;
    }

    uint16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), 2);
    uint32_t sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    uint32_t byteRate;
    file.read(reinterpret_cast<char*>(&byteRate), 4);
    uint16_t blockAlign;
    file.read(reinterpret_cast<char*>(&blockAlign), 2);
    uint16_t bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

    // Skip any extra parameters
    file.seekg(subchunk1Size - 16, std::ios_base::cur);

    // Read data subchunk
    char subchunk2ID[4];
    file.read(subchunk2ID, 4);
    if (strncmp(subchunk2ID, "data", 4) != 0) {
        std::cerr << "data subchunk not found" << std::endl;
        return false;
    }

    uint32_t subchunk2Size;
    file.read(reinterpret_cast<char*>(&subchunk2Size), 4);

    data = new char[subchunk2Size];
    file.read(static_cast<char*>(data), subchunk2Size);

    // Determine format
    if (numChannels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
    else if (numChannels == 2) {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }
    else {
        std::cerr << "Unsupported number of channels: " << numChannels << std::endl;
        delete[] static_cast<char*>(data);
        return false;
    }

    size = subchunk2Size;
    freq = sampleRate;

    return true;
}

void AudioPlayer::play(int loopCount) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isPlaying) {
        stop();
    }

    m_loopCount = loopCount;
    m_isPlaying = true;
    m_playbackThread = std::thread(&AudioPlayer::playbackLoop, this);
}

void AudioPlayer::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_isPlaying) return;
        m_isPlaying = false;
    }

    alSourceStop(m_source);
    if (m_playbackThread.joinable()) {
        m_playbackThread.join();
    }
}

void AudioPlayer::playbackLoop() {
    int loopsRemaining = m_loopCount;
    bool infiniteLoop = (loopsRemaining <= 0);

    alSourcei(m_source, AL_LOOPING, infiniteLoop ? AL_TRUE : AL_FALSE);
    alSourcePlay(m_source);

    while (m_isPlaying) {
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        if (!infiniteLoop && state != AL_PLAYING) {
            if (--loopsRemaining <= 0) break;
            alSourcePlay(m_source);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlaying = false;
}

void AudioPlayer::setLoopCount(int count) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loopCount = count;
}

void AudioPlayer::setVolume(float volume) {
    std::lock_guard<std::mutex> lock(m_mutex);
    volume = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    alSourcef(m_source, AL_GAIN, volume);
}