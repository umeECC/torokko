#include "System/AudioManager.h"

//-------------------------------------
// ‰Šú‰»
//-------------------------------------
void AudioManager::Initialize()
{
    Audio& audio = Audio::Instance();

    // ===== BGM =====
    bgms["title"] = audio.LoadAudioSource("Data/Sound/tai.wav");
    bgms["stage"] = audio.LoadAudioSource("Data/Sound/game.wav");
  

     //===== Œø‰Ê‰¹ =====
    bgms["over"] = audio.LoadAudioSource("Data/Sound/over.wav");

    bgms["clear"] = audio.LoadAudioSource("Data/Sound/clear.wav");



    // ‰Šú‰¹—ÊÝ’è
    SetBGMVolume(0.6f);
    SetSEVolume(0.8f);
}

//-------------------------------------
// I—¹
//-------------------------------------
void AudioManager::Finalize()
{
    StopBGM();

    for (auto& bgm : bgms)
    {
        delete bgm.second;
    }
    bgms.clear();

    for (auto& se : ses)
    {
        delete se.second;
    }
    ses.clear();
}

//-------------------------------------
// BGMÄ¶
//-------------------------------------
void AudioManager::PlayBGM(const std::string& name, bool loop)
{
    auto it = bgms.find(name);
    if (it == bgms.end())
        return;

    // ¡–Â‚Á‚Ä‚¢‚éBGM’âŽ~
    if (currentBGM)
    {
        currentBGM->Stop();
    }

    currentBGM = it->second;
    currentBGM->SetVolume(bgmVolume);
    currentBGM->Play(loop);
}

//-------------------------------------
// BGM’âŽ~
//-------------------------------------
void AudioManager::StopBGM()
{
    if (currentBGM)
    {
        currentBGM->Stop();
        currentBGM = nullptr;
    }
}

//-------------------------------------
// Œø‰Ê‰¹Ä¶
//-------------------------------------
void AudioManager::PlaySE(const std::string& name)
{
    auto it = ses.find(name);
    if (it == ses.end())
        return;

    it->second->SetVolume(seVolume);
    it->second->Play(false);
}

//-------------------------------------
// ‰¹—ÊÝ’è
//-------------------------------------
void AudioManager::SetBGMVolume(float volume)
{
    bgmVolume = volume;

    if (currentBGM)
    {
        currentBGM->SetVolume(volume);
    }
}

void AudioManager::SetSEVolume(float volume)
{
    seVolume = volume;
}
