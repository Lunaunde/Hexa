// PlaySound.cpp
#include "PlaySound.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// ��ʼ����̬��Ա
std::vector<PlaySound::ALResources> PlaySound::s_al_pool;
constexpr int MAX_AL_SOURCES = 32;

void PlaySound::initialize() {
    std::lock_guard lock(s_global_mutex);
    if (!s_device) {
        s_device = alcOpenDevice(nullptr);
        if (!s_device) {
            throw std::runtime_error("Failed to open OpenAL device");
        }

        s_context = alcCreateContext(s_device, nullptr);
        if (!s_context || !alcMakeContextCurrent(s_context)) {
            alcCloseDevice(s_device);
            throw std::runtime_error("Failed to create OpenAL context");
        }

        // Ԥ������Դ��
        s_al_pool.resize(MAX_AL_SOURCES);
        alGenSources(MAX_AL_SOURCES, &s_al_pool[0].source);
        alGenBuffers(MAX_AL_SOURCES, &s_al_pool[0].buffer);
        check_al_error("Resource pre-allocation");
    }
}

void PlaySound::shutdown() {
    std::lock_guard lock(s_global_mutex);
    if (s_context) {
        alcDestroyContext(s_context);
        s_context = nullptr;
    }
    if (s_device) {
        alcCloseDevice(s_device);
        s_device = nullptr;
    }
}

PlaySound::PlaySound(const std::string& filename) {
    std::lock_guard lock(s_global_mutex);

    // ϵͳ��ʼ�����
    if (!s_device) initialize();

    // ��Դ��ȡ
    auto res = acquire_al_resource();
    m_source = res.source;
    m_buffer = res.buffer;

    // ������Ƶ
    if (!load_wav(filename)) {
        release_al_resource();
        throw std::runtime_error("Failed to load audio file");
    }

    alSourcei(m_source, AL_BUFFER, m_buffer);
    check_al_error("Source buffer binding");
}

PlaySound::~PlaySound() {
    stop();
    release_al_resource();
}

PlaySound::ALResources PlaySound::acquire_al_resource() {
    std::lock_guard lock(s_global_mutex);
    if (s_active_instances >= MAX_AL_SOURCES) {
        throw std::runtime_error("Exceeded maximum audio instances");
    }

    auto it = std::find_if(s_al_pool.begin(), s_al_pool.end(),
        [](const ALResources& r) { return r.source != 0; });

    if (it == s_al_pool.end()) {
        throw std::runtime_error("No available AL resources");
    }

    ALResources res = *it;
    it->source = 0; // ���Ϊ��ʹ��
    s_active_instances++;
    return res;
}

void PlaySound::release_al_resource() {
    std::lock_guard lock(s_global_mutex);
    auto it = std::find_if(s_al_pool.begin(), s_al_pool.end(),
        [this](const ALResources& r) { return r.source == m_source; });

    if (it != s_al_pool.end()) {
        alSourceStop(m_source);
        alSourcei(m_source, AL_BUFFER, 0);
        it->source = m_source; // ������Դ
        s_active_instances--;
    }
}

bool PlaySound::load_wav(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // ��ȡRIFFͷ
    char chunkID[4];
    if (!file.read(chunkID, 4) || strncmp(chunkID, "RIFF", 4) != 0) {
        std::cerr << "Not a valid WAV file" << std::endl;
        return false;
    }

    // ����chunkSize
    file.seekg(4, std::ios_base::cur);

    // ����ʽ��ʶ
    char formatID[4];
    if (!file.read(formatID, 4) || strncmp(formatID, "WAVE", 4) != 0) {
        std::cerr << "Not a valid WAV file" << std::endl;
        return false;
    }

    // ����fmt�ӿ�
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    do {
        if (!file.read(subchunk1ID, 4)) break;
        if (strncmp(subchunk1ID, "fmt ", 4) == 0) break;

        // ����δ֪�ӿ�
        uint32_t skipSize;
        file.read(reinterpret_cast<char*>(&skipSize), 4);
        file.seekg(skipSize, std::ios_base::cur);
    } while (true);

    if (strncmp(subchunk1ID, "fmt ", 4) != 0) {
        std::cerr << "fmt subchunk not found" << std::endl;
        return false;
    }

    // ��ȡ��ʽ����
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
    file.seekg(6, std::ios_base::cur); // ����byteRate��blockAlign
    uint16_t bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

    // ����ʣ���ʽ����
    file.seekg(subchunk1Size - 16, std::ios_base::cur);

    // ����data�ӿ�
    char subchunk2ID[4];
    do {
        if (!file.read(subchunk2ID, 4)) break;
        if (strncmp(subchunk2ID, "data", 4) == 0) break;

        // ����δ֪�ӿ�
        uint32_t skipSize;
        file.read(reinterpret_cast<char*>(&skipSize), 4);
        file.seekg(skipSize, std::ios_base::cur);
    } while (true);

    if (strncmp(subchunk2ID, "data", 4) != 0) {
        std::cerr << "data subchunk not found" << std::endl;
        return false;
    }

    // ��ȡ��Ƶ����
    uint32_t subchunk2Size;
    file.read(reinterpret_cast<char*>(&subchunk2Size), 4);
    char* audioData = new char[subchunk2Size];
    if (!file.read(audioData, subchunk2Size)) {
        std::cerr << "Failed to read audio data" << std::endl;
        delete[] audioData;
        return false;
    }

    // ȷ��OpenAL��ʽ
    ALenum format = AL_NONE;
    if (numChannels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
    else if (numChannels == 2) {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }
    else {
        std::cerr << "Unsupported number of channels: " << numChannels << std::endl;
        delete[] audioData;
        return false;
    }

    // �������ݵ�OpenAL������
    {
        std::lock_guard lock(s_global_mutex);
        alBufferData(m_buffer, format, audioData, subchunk2Size, sampleRate);
        if (!check_al_error("Buffer data loading")) {
            delete[] audioData;
            return false;
        }
    }

    delete[] audioData;
    return true;
}

void PlaySound::play(int loops) {
    stop(); // ȷ��ֹ֮ͣǰ�Ĳ���

    {
        std::lock_guard lock(m_instance_mutex);
        m_should_play = true;
        m_loop_count = loops;
    }

    m_play_thread = std::thread([this] {
        playback_loop(m_loop_count);
        });
}

void PlaySound::stop() {
    {
        std::lock_guard lock(m_instance_mutex);
        m_should_play = false;
    }

    if (m_play_thread.joinable()) {
        m_play_thread.join();
    }

    std::lock_guard lock(s_global_mutex);
    alSourceStop(m_source);
}

void PlaySound::playback_loop(int loops) {
    bool infinite = loops <= 0;
    int remaining = infinite ? 1 : loops;

    while (remaining > 0 && m_should_play) {
        {
            std::lock_guard lock(s_global_mutex);
            alSourcei(m_source, AL_LOOPING, infinite ? AL_TRUE : AL_FALSE);
            alSourcePlay(m_source);
            check_al_error("Source play");
        }

        ALint state;
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            {
                std::lock_guard lock(s_global_mutex);
                alGetSourcei(m_source, AL_SOURCE_STATE, &state);
            }
        } while (m_should_play && state == AL_PLAYING);

        if (!infinite) remaining--;
    }
}

void PlaySound::set_volume(float volume) {
    volume = std::clamp(volume, 0.0f, 1.0f);
    {
        std::lock_guard lock(m_instance_mutex);
        m_volume = volume;
    }

    std::lock_guard lock(s_global_mutex);
    alSourcef(m_source, AL_GAIN, volume);
    check_al_error("Set volume");
}

bool PlaySound::is_playing() const {
    ALint state;
    {
        std::lock_guard lock(s_global_mutex);
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    }
    return state == AL_PLAYING;
}

bool PlaySound::check_al_error(const char* operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL Error (" << operation << "): "
            << alGetString(error) << std::endl;
        return false;
    }
    return true;
