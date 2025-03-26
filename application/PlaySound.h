// PlaySound.h
#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

class PlaySound {
public:
    explicit PlaySound(const std::string& filename);
    ~PlaySound();

    void play(int loops = 1);
    void stop();
    void set_volume(float volume); // 0.0~1.0
    bool is_playing() const;

    // ϵͳ������
    static void initialize();
    static void shutdown();

private:
    struct ALResources {
        ALuint source;
        ALuint buffer;
    };

    // ��̬��Դ����
    static inline ALCdevice* s_device = nullptr;
    static inline ALCcontext* s_context = nullptr;
    static inline std::mutex s_global_mutex;
    static inline std::atomic<int> s_active_instances{ 0 };
    static inline std::vector<ALResources> s_al_pool;

    // ʵ����Դ
    ALuint m_source;
    ALuint m_buffer;
    std::thread m_play_thread;
    mutable std::mutex m_instance_mutex;
    std::atomic<bool> m_should_play{ false };
    int m_loop_count;
    float m_volume = 1.0f;

    // ˽�з���
    bool load_wav(const std::string& filename);
    void playback_loop(int loops);
    ALResources acquire_al_resource();
    void release_al_resource();

    // OpenAL ������
    static bool check_al_error(const char* operation);
};