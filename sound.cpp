

//#include "Sound.h"
//#include <fstream>
//#include <vector>
//#include <wrl/client.h>
//#include <xaudio2.h>
//
//using Microsoft::WRL::ComPtr;
//
//ComPtr<IXAudio2> g_pXAudio2;
//IXAudio2MasteringVoice* g_pMasterVoice = nullptr;
//IXAudio2SourceVoice* g_pSourceVoice = nullptr;
//std::vector<BYTE> g_audioData;
//
//static HRESULT LoadAudioData(const wchar_t* szFilename, std::vector<BYTE>& audioData)
//{
//    std::ifstream file(szFilename, std::ios::binary | std::ios::ate);
//    if (!file.is_open()) {
//        return E_FAIL;
//    }
//
//    std::streamsize size = file.tellg();
//    file.seekg(0, std::ios::beg);
//
//    audioData.resize(size);
//    if (!file.read(reinterpret_cast<char*>(audioData.data()), size)) {
//        return E_FAIL;
//    }
//
//    return S_OK;
//}
//
//void InitSound()
//{
//    HRESULT hr = XAudio2Create(&g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
//    if (FAILED(hr)) {
//        return;
//    }
//
//    hr = g_pXAudio2->CreateMasteringVoice(&g_pMasterVoice);
//    if (FAILED(hr)) {
//        return;
//    }
//}
//
//void UninitSound()
//{
//    if (g_pSourceVoice)
//    {
//        g_pSourceVoice->DestroyVoice();
//        g_pSourceVoice = nullptr;
//    }
//
//    if (g_pMasterVoice)
//    {
//        g_pMasterVoice->DestroyVoice();
//        g_pMasterVoice = nullptr;
//    }
//
//    if (g_pXAudio2)
//    {
//        g_pXAudio2.Reset();
//    }
//}
//
//void PlayBGM(const wchar_t* filename)
//{
//    StopBGM();
//
//    HRESULT hr;
//
//    if (!g_pXAudio2) {
//        return;
//    }
//
//    hr = LoadAudioData(filename, g_audioData);
//    if (FAILED(hr)) {
//        return;
//    }
//
//    WAVEFORMATEX wfx = { 0 };
//    wfx.wFormatTag = WAVE_FORMAT_PCM;
//    wfx.nChannels = 2;
//    wfx.nSamplesPerSec = 44100;
//    wfx.nAvgBytesPerSec = 44100 * 2 * 16 / 8;
//    wfx.nBlockAlign = 2 * 16 / 8;
//    wfx.wBitsPerSample = 16;
//    wfx.cbSize = 0;
//
//    hr = g_pXAudio2->CreateSourceVoice(&g_pSourceVoice, &wfx);
//    if (FAILED(hr)) {
//        return;
//    }
//
//    XAUDIO2_BUFFER buffer = { 0 };
//    buffer.AudioBytes = static_cast<UINT32>(g_audioData.size());
//    buffer.pAudioData = g_audioData.data();
//    buffer.Flags = XAUDIO2_END_OF_STREAM;
//    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
//
//    hr = g_pSourceVoice->SubmitSourceBuffer(&buffer);
//    if (FAILED(hr)) {
//        return;
//    }
//
//    g_pSourceVoice->Start(0);
//}
//
//void StopBGM()
//{
//    if (g_pSourceVoice)
//    {
//        g_pSourceVoice->Stop(0);
//        g_pSourceVoice->DestroyVoice();
//        g_pSourceVoice = nullptr;
//    }
//}