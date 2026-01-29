#pragma once

#include <unordered_map>
#include <string>

#include "System/Audio.h"

// ゲーム側用の音管理クラス
class AudioManager
{
private:
    AudioManager() = default;
    ~AudioManager() = default;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

public:
    // インスタンス取得
    static AudioManager& Instance()
    {
        static AudioManager instance;
        return instance;
    }

    // 初期化・終了
    void Initialize();
    void Finalize();

    // BGM制御
    void PlayBGM(const std::string& name, bool loop = true);
    void StopBGM();

    // 効果音
    void PlaySE(const std::string& name);

    // カテゴリ音量
    void SetBGMVolume(float volume);
    void SetSEVolume(float volume);

private:
    std::unordered_map<std::string, AudioSource*> bgms;
    std::unordered_map<std::string, AudioSource*> ses;

    AudioSource* currentBGM = nullptr;

    float bgmVolume = 1.0f;
    float seVolume = 1.0f;
};
