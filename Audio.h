
//このエラーの原因は、「Audio.」コードのどこかにあります。
//Error	C2027	use of undefined type 'DirectX::DynamicSoundEffectInstance::Impl'	
//Error	C2338	static_assert failed: 'can't delete an incomplete type'	


//--------------------------------------------------------------------------------------
// File: Audio.h
//
// DirectXTK for Audio header
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include <memory>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "windows.h"
#include <objbase.h>
#include <mmreg.h>
#include <Audioclient.h>

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
#include <xma2defs.h>
#pragma comment(lib,"acphal.lib")
#endif

#ifndef XAUDIO2_HELPER_FUNCTIONS
#define XAUDIO2_HELPER_FUNCTIONS
#endif

#if defined(USING_XAUDIO2_REDIST) || (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/) || defined(_XBOX_ONE)
#define USING_XAUDIO2_9
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#define USING_XAUDIO2_8
#elif (_WIN32_WINNT >= 0x0601 /*_WIN32_WINNT_WIN7*/)
#error Windows 7 SP1 requires the XAudio2Redist NuGet package https://aka.ms/xaudio2redist
#else
#error DirectX Tool Kit for Audio not supported on this platform
#endif

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>

#ifndef USING_XAUDIO2_REDIST
#if defined(USING_XAUDIO2_8) && defined(NTDDI_WIN10) && !defined(_M_IX86)
// The xaudio2_8.lib in the Windows 10 SDK for x86 is incorrectly annotated as __cdecl instead of __stdcall, so avoid using it in this case.
// Windows 10 SDK の x86 用の xaudio2_8.lib は、__cdecl として誤って注釈されており、本来は __stdcall であるため、この場合は使用を避けます。
#pragma comment(lib,"xaudio2_8.lib")
#else
#pragma comment(lib,"xaudio2.lib")
#endif
#endif

#include <DirectXMath.h>

namespace DirectX
{
    class SoundEffectInstance;
    class SoundStreamInstance;

    //----------------------------------------------------------------------------------
    struct AudioStatistics
    {
        size_t  playingOneShots;        // Number of one-shot sounds currently playing
        // 現在再生中のワンショットサウンドの数
        size_t  playingInstances;       // Number of sound effect instances currently playing
        // 現在再生中のサウンドエフェクトインスタンスの数
        size_t  allocatedInstances;     // Number of SoundEffectInstance allocated
        // 割り当てられた SoundEffectInstance の数
        size_t  allocatedVoices;        // Number of XAudio2 voices allocated (standard, 3D, one-shots, and idle one-shots)
        // 割り当てられた XAudio2 のボイス数 (標準、3D、ワンショット、アイドルワンショット)
        size_t  allocatedVoices3d;      // Number of XAudio2 voices allocated for 3D
        // 3D 用に割り当てられた XAudio2 のボイス数
        size_t  allocatedVoicesOneShot; // Number of XAudio2 voices allocated for one-shot sounds
        // ワンショットサウンド用に割り当てられた XAudio2 のボイス数
        size_t  allocatedVoicesIdle;    // Number of XAudio2 voices allocated for one-shot sounds but not currently in use
        // ワンショットサウンド用に割り当てられているが現在使用されていない XAudio2 のボイス数
        size_t  audioBytes;             // Total wave data (in bytes) in SoundEffects and in-memory WaveBanks
        // SoundEffects およびメモリ内 WaveBanks に含まれる全波形データの合計サイズ (バイト単位)
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        size_t  xmaAudioBytes;          // Total wave data (in bytes) in SoundEffects and in-memory WaveBanks allocated with ApuAlloc
        // ApuAlloc を使用して割り当てられた SoundEffects およびメモリ内 WaveBanks の全波形データ (バイト単位)
#endif
        size_t  streamingBytes;         // Total size of streaming buffers (in bytes) in streaming WaveBanks
        // ストリーミング WaveBanks 内のストリーミングバッファの合計サイズ (バイト単位)
    };

    //----------------------------------------------------------------------------------
    class IVoiceNotify
    {
    public:
        virtual ~IVoiceNotify() = default;

        IVoiceNotify(const IVoiceNotify&) = delete;
        IVoiceNotify& operator=(const IVoiceNotify&) = delete;

        IVoiceNotify(IVoiceNotify&&) = default;
        IVoiceNotify& operator=(IVoiceNotify&&) = default;

        virtual void __cdecl OnBufferEnd() = 0;
        // Notification that a voice buffer has finished
        // ボイスバッファの再生が終了したことを通知
        // Note this is called from XAudio2's worker thread, so it should perform very minimal and thread-safe operations
        // この関数は XAudio2 のワーカースレッドから呼び出されるため、非常に最小限かつスレッドセーフな操作に留めるべきです

        virtual void __cdecl OnCriticalError() = 0;
        // Notification that the audio engine encountered a critical error
        // オーディオエンジンで重大なエラーが発生したことを通知

        virtual void __cdecl OnReset() = 0;
        // Notification of an audio engine reset
        // オーディオエンジンのリセットを通知

        virtual void __cdecl OnUpdate() = 0;
        // Notification of an audio engine per-frame update (opt-in)
        // オーディオエンジンのフレームごとの更新を通知 (オプトイン)

        virtual void __cdecl OnDestroyEngine() noexcept = 0;
        // Notification that the audio engine is being destroyed
        // オーディオエンジンが破棄されることを通知

        virtual void __cdecl OnTrim() = 0;
        // Notification of a request to trim the voice pool
        // ボイスプールのトリムリクエストを通知

        virtual void __cdecl GatherStatistics(AudioStatistics& stats) const = 0;
        // Contribute to statistics request
        // 統計情報の収集に貢献

        virtual void __cdecl OnDestroyParent() noexcept = 0;
        // Optional notification used by some objects
        // 一部のオブジェクトで使用されるオプションの通知

    protected:
        IVoiceNotify() = default;
    };

    //----------------------------------------------------------------------------------
    enum AUDIO_ENGINE_FLAGS : uint32_t
    {
        AudioEngine_Default             = 0x0,

        AudioEngine_EnvironmentalReverb = 0x1,
        AudioEngine_ReverbUseFilters    = 0x2,
        AudioEngine_UseMasteringLimiter = 0x4,

        AudioEngine_Debug               = 0x10000,
        AudioEngine_ThrowOnNoAudioHW    = 0x20000,
        AudioEngine_DisableVoiceReuse   = 0x40000,
    };

    enum SOUND_EFFECT_INSTANCE_FLAGS : uint32_t
    {
        SoundEffectInstance_Default             = 0x0,

        SoundEffectInstance_Use3D               = 0x1,
        SoundEffectInstance_ReverbUseFilters    = 0x2,
        SoundEffectInstance_NoSetPitch          = 0x4,

        SoundEffectInstance_UseRedirectLFE      = 0x10000,
    };

    enum AUDIO_ENGINE_REVERB : unsigned int
    {
        Reverb_Off,
        Reverb_Default,
        Reverb_Generic,
        Reverb_Forest,
        Reverb_PaddedCell,
        Reverb_Room,
        Reverb_Bathroom,
        Reverb_LivingRoom,
        Reverb_StoneRoom,
        Reverb_Auditorium,
        Reverb_ConcertHall,
        Reverb_Cave,
        Reverb_Arena,
        Reverb_Hangar,
        Reverb_CarpetedHallway,
        Reverb_Hallway,
        Reverb_StoneCorridor,
        Reverb_Alley,
        Reverb_City,
        Reverb_Mountains,
        Reverb_Quarry,
        Reverb_Plain,
        Reverb_ParkingLot,
        Reverb_SewerPipe,
        Reverb_Underwater,
        Reverb_SmallRoom,
        Reverb_MediumRoom,
        Reverb_LargeRoom,
        Reverb_MediumHall,
        Reverb_LargeHall,
        Reverb_Plate,
        Reverb_MAX
    };

    enum SoundState
    {
        STOPPED = 0,
        PLAYING,
        PAUSED
    };


    //----------------------------------------------------------------------------------
    class AudioEngine
    {
    public:
        explicit AudioEngine(
            AUDIO_ENGINE_FLAGS flags = AudioEngine_Default,
            _In_opt_ const WAVEFORMATEX* wfx = nullptr,
            _In_opt_z_ const wchar_t* deviceId = nullptr,
            AUDIO_STREAM_CATEGORY category = AudioCategory_GameEffects) noexcept(false);

        AudioEngine(AudioEngine&& moveFrom) noexcept;
        AudioEngine& operator= (AudioEngine&& moveFrom) noexcept;

        AudioEngine(AudioEngine const&) = delete;
        AudioEngine& operator= (AudioEngine const&) = delete;

        virtual ~AudioEngine();

        bool __cdecl Update();
        // Performs per-frame processing for the audio engine, returns false if in 'silent mode'
        // オーディオエンジンのフレームごとの処理を実行します。'サイレントモード'の場合は false を返します。

        bool __cdecl Reset(_In_opt_ const WAVEFORMATEX* wfx = nullptr, _In_opt_z_ const wchar_t* deviceId = nullptr);
        // Reset audio engine from critical error/silent mode using a new device; can also 'migrate' the graph
        // Returns true if succesfully reset, false if in 'silent mode' due to no default device
        // Note: One shots are lost, all SoundEffectInstances are in the STOPPED state after successful reset
        // 新しいデバイスを使用してクリティカルエラー/サイレントモードからオーディオエンジンをリセットします。また、グラフを移行することも可能です。
        // リセットが成功した場合は true を返し、デフォルトデバイスがないためサイレントモードにある場合は false を返します。
        // 注意: ワンショットは失われ、リセット後すべての SoundEffectInstance は停止状態 (STOPPED) になります。

        void __cdecl Suspend() noexcept;
        void __cdecl Resume();
        // Suspend/resumes audio processing (i.e. global pause/resume)
        // オーディオ処理を一時停止/再開します (グローバルなポーズ/再開)

        float __cdecl GetMasterVolume() const noexcept;
        void __cdecl SetMasterVolume(float volume);
        // Master volume property for all sounds
        // すべてのサウンドのマスターボリュームプロパティ

        void __cdecl SetReverb(AUDIO_ENGINE_REVERB reverb);
        void __cdecl SetReverb(_In_opt_ const XAUDIO2FX_REVERB_PARAMETERS* native);
        // Sets environmental reverb for 3D positional audio (if active)
        // 3D定位オーディオの環境リバーブを設定します (有効な場合)

        void __cdecl SetMasteringLimit(int release, int loudness);
        // Sets the mastering volume limiter properties (if active)
        // マスターボリュームリミッターのプロパティを設定します (有効な場合)

        AudioStatistics __cdecl GetStatistics() const;
        // Gathers audio engine statistics
        // オーディオエンジンの統計情報を収集します

        WAVEFORMATEXTENSIBLE __cdecl GetOutputFormat() const noexcept;
        // Returns the format consumed by the mastering voice (which is the same as the device output if defaults are used)
        // マスターボイスが使用するフォーマットを返します (デフォルト設定の場合、デバイス出力と同じ)

        uint32_t __cdecl GetChannelMask() const noexcept;
        // Returns the output channel mask
        // 出力チャネルマスクを返します

        unsigned int __cdecl GetOutputChannels() const noexcept;
        // Returns the number of output channels
        // 出力チャネルの数を返します

        bool __cdecl IsAudioDevicePresent() const noexcept;
        // Returns true if the audio graph is operating normally, false if in 'silent mode'
        // オーディオグラフが正常に動作している場合は true を返し、'サイレントモード' の場合は false を返します。

        bool __cdecl IsCriticalError() const noexcept;
        // Returns true if the audio graph is halted due to a critical error (which also places the engine into 'silent mode')
        // クリティカルエラーによりオーディオグラフが停止している場合は true を返します (エンジンも 'サイレントモード' に入ります)。

    // Voice pool management.
        void __cdecl SetDefaultSampleRate(int sampleRate);
        // Sample rate for voices in the reuse pool (defaults to 44100)
        // 再利用プール内のボイスのサンプルレート (デフォルトは 44100)

        void __cdecl SetMaxVoicePool(size_t maxOneShots, size_t maxInstances);
        // Maximum number of voices to allocate for one-shots and instances
        // Note: one-shots over this limit are ignored; too many instance voices throws an exception
        // ワンショットとインスタンスのために割り当てるボイスの最大数
        // 注意: この制限を超えるワンショットは無視され、インスタンスボイスが多すぎる場合は例外がスローされます。

        void __cdecl TrimVoicePool();
        // Releases any currently unused voices
        // 現在使用されていないボイスを解放します

    // Internal-use functions
        void __cdecl AllocateVoice(_In_ const WAVEFORMATEX* wfx,
            SOUND_EFFECT_INSTANCE_FLAGS flags, bool oneshot, _Outptr_result_maybenull_ IXAudio2SourceVoice** voice);

        void __cdecl DestroyVoice(_In_ IXAudio2SourceVoice* voice) noexcept;
        // Should only be called for instance voices, not one-shots
        // インスタンスボイスのみに使用し、ワンショットには使用しないでください。

        void __cdecl RegisterNotify(_In_ IVoiceNotify* notify, bool usesUpdate);
        void __cdecl UnregisterNotify(_In_ IVoiceNotify* notify, bool usesOneShots, bool usesUpdate);

        // XAudio2 interface access
        IXAudio2* __cdecl GetInterface() const noexcept;
        IXAudio2MasteringVoice* __cdecl GetMasterVoice() const noexcept;
        IXAudio2SubmixVoice* __cdecl GetReverbVoice() const noexcept;
        X3DAUDIO_HANDLE& __cdecl Get3DHandle() const noexcept;

        // Static functions
        struct RendererDetail
        {
            std::wstring deviceId;
            std::wstring description;
        };

        static std::vector<RendererDetail> __cdecl GetRendererDetails();
        // Returns a list of valid audio endpoint devices
        // 有効なオーディオエンドポイントデバイスのリストを返します。

    private:
        // Private implementation.
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

    //----------------------------------------------------------------------------------
    class WaveBank
    {
    public:
        WaveBank(_In_ AudioEngine* engine, _In_z_ const wchar_t* wbFileName);

        WaveBank(WaveBank&& moveFrom) noexcept;
        WaveBank& operator= (WaveBank&& moveFrom) noexcept;

        WaveBank(WaveBank const&) = delete;
        WaveBank& operator= (WaveBank const&) = delete;

        virtual ~WaveBank();

        void __cdecl Play(unsigned int index);
        void __cdecl Play(unsigned int index, float volume, float pitch, float pan);

        void __cdecl Play(_In_z_ const char* name);
        void __cdecl Play(_In_z_ const char* name, float volume, float pitch, float pan);

        std::unique_ptr<SoundEffectInstance> __cdecl CreateInstance(unsigned int index,
            SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);
        std::unique_ptr<SoundEffectInstance> __cdecl CreateInstance(_In_z_ const char* name,
            SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);

        std::unique_ptr<SoundStreamInstance> __cdecl CreateStreamInstance(unsigned int index,
            SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);
        std::unique_ptr<SoundStreamInstance> __cdecl CreateStreamInstance(_In_z_ const char* name,
            SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);

        bool __cdecl IsPrepared() const noexcept;
        bool __cdecl IsInUse() const noexcept;
        bool __cdecl IsStreamingBank() const noexcept;

        size_t __cdecl GetSampleSizeInBytes(unsigned int index) const noexcept;
        // Returns size of wave audio data

        size_t __cdecl GetSampleDuration(unsigned int index) const noexcept;
        // Returns the duration in samples

        size_t __cdecl GetSampleDurationMS(unsigned int index) const noexcept;
        // Returns the duration in milliseconds

        const WAVEFORMATEX* __cdecl GetFormat(unsigned int index, _Out_writes_bytes_(maxsize) WAVEFORMATEX* wfx, size_t maxsize) const noexcept;

        int __cdecl Find(_In_z_ const char* name) const;

#ifdef USING_XAUDIO2_9
        bool __cdecl FillSubmitBuffer(unsigned int index, _Out_ XAUDIO2_BUFFER& buffer, _Out_ XAUDIO2_BUFFER_WMA& wmaBuffer) const;
#else
        void __cdecl FillSubmitBuffer(unsigned int index, _Out_ XAUDIO2_BUFFER& buffer) const;
#endif

        void __cdecl UnregisterInstance(_In_ IVoiceNotify* instance);

        HANDLE __cdecl GetAsyncHandle() const noexcept;

        bool __cdecl GetPrivateData(unsigned int index, _Out_writes_bytes_(datasize) void* data, size_t datasize);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;
    };

    //----------------------------------------------------------------------------------
    class SoundEffect
    {
    public:
        SoundEffect(_In_ AudioEngine* engine, _In_z_ const wchar_t* waveFileName);

        SoundEffect(_In_ AudioEngine* engine, _Inout_ std::unique_ptr<uint8_t[]>& wavData,
            _In_ const WAVEFORMATEX* wfx, _In_reads_bytes_(audioBytes) const uint8_t* startAudio, size_t audioBytes);

        SoundEffect(_In_ AudioEngine* engine, _Inout_ std::unique_ptr<uint8_t[]>& wavData,
            _In_ const WAVEFORMATEX* wfx, _In_reads_bytes_(audioBytes) const uint8_t* startAudio, size_t audioBytes,
            uint32_t loopStart, uint32_t loopLength);

#ifdef USING_XAUDIO2_9

        SoundEffect(_In_ AudioEngine* engine, _Inout_ std::unique_ptr<uint8_t[]>& wavData,
            _In_ const WAVEFORMATEX* wfx, _In_reads_bytes_(audioBytes) const uint8_t* startAudio, size_t audioBytes,
            _In_reads_(seekCount) const uint32_t* seekTable, size_t seekCount);

#endif

        SoundEffect(SoundEffect&& moveFrom) noexcept;
        SoundEffect& operator= (SoundEffect&& moveFrom) noexcept;

        SoundEffect(SoundEffect const&) = delete;
        SoundEffect& operator= (SoundEffect const&) = delete;

        virtual ~SoundEffect();

        void __cdecl Play();
        void __cdecl Play(float volume, float pitch, float pan);

        std::unique_ptr<SoundEffectInstance> __cdecl CreateInstance(SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);

        bool __cdecl IsInUse() const noexcept;

        size_t __cdecl GetSampleSizeInBytes() const noexcept;
        // Returns size of wave audio data
        // 波形オーディオデータのサイズを返します

        size_t __cdecl GetSampleDuration() const noexcept;
        // Returns the duration in samples
        // サンプル単位で期間を返します

        size_t __cdecl GetSampleDurationMS() const noexcept;
        // Returns the duration in milliseconds
        // ミリ秒単位で期間を返します

        const WAVEFORMATEX* __cdecl GetFormat() const noexcept;

#ifdef USING_XAUDIO2_9
        bool __cdecl FillSubmitBuffer(_Out_ XAUDIO2_BUFFER& buffer, _Out_ XAUDIO2_BUFFER_WMA& wmaBuffer) const;
#else
        void __cdecl FillSubmitBuffer(_Out_ XAUDIO2_BUFFER& buffer) const;
#endif

        void __cdecl UnregisterInstance(_In_ IVoiceNotify* instance);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;
    };


    //----------------------------------------------------------------------------------
    struct AudioListener : public X3DAUDIO_LISTENER
    {
        AudioListener() noexcept
        {
            memset(this, 0, sizeof(X3DAUDIO_LISTENER));

            OrientFront.z = -1.f;

            OrientTop.y = 1.f;
        }

        void XM_CALLCONV SetPosition(FXMVECTOR v) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Position), v);
        }
        void __cdecl SetPosition(const XMFLOAT3& pos) noexcept
        {
            Position.x = pos.x;
            Position.y = pos.y;
            Position.z = pos.z;
        }

        void XM_CALLCONV SetVelocity(FXMVECTOR v) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Velocity), v);
        }
        void __cdecl SetVelocity(const XMFLOAT3& vel) noexcept
        {
            Velocity.x = vel.x;
            Velocity.y = vel.y;
            Velocity.z = vel.z;
        }

        void XM_CALLCONV SetOrientation(FXMVECTOR forward, FXMVECTOR up) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), forward);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), up);
        }
        void __cdecl SetOrientation(const XMFLOAT3& forward, const XMFLOAT3& up) noexcept
        {
            OrientFront.x = forward.x;  OrientTop.x = up.x;
            OrientFront.y = forward.y;  OrientTop.y = up.y;
            OrientFront.z = forward.z;  OrientTop.z = up.z;
        }

        void XM_CALLCONV SetOrientationFromQuaternion(FXMVECTOR quat) noexcept
        {
            XMVECTOR forward = XMVector3Rotate(g_XMIdentityR2, quat);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), forward);

            XMVECTOR up = XMVector3Rotate(g_XMIdentityR1, quat);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), up);
        }

        void XM_CALLCONV Update(FXMVECTOR newPos, XMVECTOR upDir, float dt) noexcept
            // Updates velocity and orientation by tracking changes in position over time...
            // 時間の経過に伴う位置の変化を追跡して、速度と方向を更新します...
        {
            if (dt > 0.f)
            {
                XMVECTOR lastPos = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&Position));

                XMVECTOR vDelta = XMVectorSubtract(newPos, lastPos);
                XMVECTOR vt = XMVectorReplicate(dt);
                XMVECTOR v = XMVectorDivide(vDelta, vt);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Velocity), v);

                vDelta = XMVector3Normalize(vDelta);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), vDelta);

                v = XMVector3Cross(upDir, vDelta);
                v = XMVector3Normalize(v);

                v = XMVector3Cross(vDelta, v);
                v = XMVector3Normalize(v);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), v);

                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Position), newPos);
            }
        }
    };

    //----------------------------------------------------------------------------------
    struct AudioEmitter : public X3DAUDIO_EMITTER
    {
        float       EmitterAzimuths[XAUDIO2_MAX_AUDIO_CHANNELS];

        AudioEmitter() noexcept :
            EmitterAzimuths{}
        {
            memset(this, 0, sizeof(X3DAUDIO_EMITTER));

            OrientFront.z = -1.f;

            OrientTop.y =
                ChannelRadius =
                CurveDistanceScaler =
                DopplerScaler = 1.f;

            ChannelCount = 1;
            pChannelAzimuths = EmitterAzimuths;

            InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
        }

        void XM_CALLCONV SetPosition(FXMVECTOR v) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Position), v);
        }
        void __cdecl SetPosition(const XMFLOAT3& pos) noexcept
        {
            Position.x = pos.x;
            Position.y = pos.y;
            Position.z = pos.z;
        }

        void XM_CALLCONV SetVelocity(FXMVECTOR v) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Velocity), v);
        }
        void __cdecl SetVelocity(const XMFLOAT3& vel) noexcept
        {
            Velocity.x = vel.x;
            Velocity.y = vel.y;
            Velocity.z = vel.z;
        }

        void XM_CALLCONV SetOrientation(FXMVECTOR forward, FXMVECTOR up) noexcept
        {
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), forward);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), up);
        }
        void __cdecl SetOrientation(const XMFLOAT3& forward, const XMFLOAT3& up) noexcept
        {
            OrientFront.x = forward.x;  OrientTop.x = up.x;
            OrientFront.y = forward.y;  OrientTop.y = up.y;
            OrientFront.z = forward.z;  OrientTop.z = up.z;
        }

        void XM_CALLCONV SetOrientationFromQuaternion(FXMVECTOR quat) noexcept
        {
            XMVECTOR forward = XMVector3Rotate(g_XMIdentityR2, quat);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), forward);

            XMVECTOR up = XMVector3Rotate(g_XMIdentityR1, quat);
            XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), up);
        }

        void XM_CALLCONV Update(FXMVECTOR newPos, XMVECTOR upDir, float dt) noexcept
            // Updates velocity and orientation by tracking changes in position over time...
            // 時間の経過に伴う位置の変化を追跡して、速度と方向を更新します...
        {
            if (dt > 0.f)
            {
                XMVECTOR lastPos = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&Position));

                XMVECTOR vDelta = XMVectorSubtract(newPos, lastPos);
                XMVECTOR vt = XMVectorReplicate(dt);
                XMVECTOR v = XMVectorDivide(vDelta, vt);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Velocity), v);

                vDelta = XMVector3Normalize(vDelta);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientFront), vDelta);

                v = XMVector3Cross(upDir, vDelta);
                v = XMVector3Normalize(v);

                v = XMVector3Cross(vDelta, v);
                v = XMVector3Normalize(v);
                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&OrientTop), v);

                XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&Position), newPos);
            }
        }
    };

    //----------------------------------------------------------------------------------
    class SoundEffectInstance
    {
    public:
        SoundEffectInstance(SoundEffectInstance&& moveFrom) noexcept;
        SoundEffectInstance& operator= (SoundEffectInstance&& moveFrom) noexcept;

        SoundEffectInstance(SoundEffectInstance const&) = delete;
        SoundEffectInstance& operator= (SoundEffectInstance const&) = delete;

        virtual ~SoundEffectInstance();

        void __cdecl Play(bool loop = false);
        void __cdecl Stop(bool immediate = true) noexcept;
        void __cdecl Pause() noexcept;
        void __cdecl Resume();

        void __cdecl SetVolume(float volume);
        void __cdecl SetPitch(float pitch);
        void __cdecl SetPan(float pan);

        void __cdecl Apply3D(const AudioListener& listener, const AudioEmitter& emitter, bool rhcoords = true);

        bool __cdecl IsLooped() const noexcept;

        SoundState __cdecl GetState() noexcept;

        IVoiceNotify* __cdecl GetVoiceNotify() const noexcept;

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;

        // Private constructors
        // プライベートコンストラクタ
        SoundEffectInstance(_In_ AudioEngine* engine, _In_ SoundEffect* effect, SOUND_EFFECT_INSTANCE_FLAGS flags);
        SoundEffectInstance(_In_ AudioEngine* engine, _In_ WaveBank* effect, unsigned int index, SOUND_EFFECT_INSTANCE_FLAGS flags);

        friend std::unique_ptr<SoundEffectInstance> __cdecl SoundEffect::CreateInstance(SOUND_EFFECT_INSTANCE_FLAGS);
        friend std::unique_ptr<SoundEffectInstance> __cdecl WaveBank::CreateInstance(unsigned int, SOUND_EFFECT_INSTANCE_FLAGS);
    };

    //----------------------------------------------------------------------------------
    class SoundStreamInstance
    {
    public:
        SoundStreamInstance(SoundStreamInstance&& moveFrom) noexcept;
        SoundStreamInstance& operator= (SoundStreamInstance&& moveFrom) noexcept;

        SoundStreamInstance(SoundStreamInstance const&) = delete;
        SoundStreamInstance& operator= (SoundStreamInstance const&) = delete;

        virtual ~SoundStreamInstance();

        void __cdecl Play(bool loop = false);
        void __cdecl Stop(bool immediate = true) noexcept;
        void __cdecl Pause() noexcept;
        void __cdecl Resume();

        void __cdecl SetVolume(float volume);
        void __cdecl SetPitch(float pitch);
        void __cdecl SetPan(float pan);

        void __cdecl Apply3D(const AudioListener& listener, const AudioEmitter& emitter, bool rhcoords = true);

        bool __cdecl IsLooped() const noexcept;

        SoundState __cdecl GetState() noexcept;

        IVoiceNotify* __cdecl GetVoiceNotify() const noexcept;

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;

        // Private constructors
        SoundStreamInstance(_In_ AudioEngine* engine, _In_ WaveBank* effect, unsigned int index, SOUND_EFFECT_INSTANCE_FLAGS flags);

        friend std::unique_ptr<SoundStreamInstance> __cdecl WaveBank::CreateStreamInstance(unsigned int, SOUND_EFFECT_INSTANCE_FLAGS);
    };

    //----------------------------------------------------------------------------------
    class DynamicSoundEffectInstance
    {
    public:
        DynamicSoundEffectInstance(_In_ AudioEngine* engine,
            _In_opt_ std::function<void __cdecl(DynamicSoundEffectInstance*)> bufferNeeded,
            int sampleRate, int channels, int sampleBits = 16,
            SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);
        DynamicSoundEffectInstance(DynamicSoundEffectInstance&& moveFrom) noexcept;
        DynamicSoundEffectInstance& operator= (DynamicSoundEffectInstance&& moveFrom) noexcept;

        DynamicSoundEffectInstance(DynamicSoundEffectInstance const&) = delete;
        DynamicSoundEffectInstance& operator= (DynamicSoundEffectInstance const&) = delete;

        virtual ~DynamicSoundEffectInstance();

        void __cdecl Play();
        void __cdecl Stop(bool immediate = true) noexcept;
        void __cdecl Pause() noexcept;
        void __cdecl Resume();

        void __cdecl SetVolume(float volume);
        void __cdecl SetPitch(float pitch);
        void __cdecl SetPan(float pan);

        void __cdecl Apply3D(const AudioListener& listener, const AudioEmitter& emitter, bool rhcoords = true);

        void __cdecl SubmitBuffer(_In_reads_bytes_(audioBytes) const uint8_t* pAudioData, size_t audioBytes);
        void __cdecl SubmitBuffer(_In_reads_bytes_(audioBytes) const uint8_t* pAudioData, uint32_t offset, size_t audioBytes);

        SoundState __cdecl GetState() noexcept;

        size_t __cdecl GetSampleDuration(size_t bytes) const noexcept;
        // Returns duration in samples of a buffer of a given size
        // 指定されたサイズのバッファのサンプルの期間を返します。

        size_t __cdecl GetSampleDurationMS(size_t bytes) const noexcept;
        // Returns duration in milliseconds of a buffer of a given size
        // 指定されたサイズのバッファの継続時間をミリ秒単位で返します。

        size_t __cdecl GetSampleSizeInBytes(uint64_t duration) const noexcept;
        // Returns size of a buffer for a duration given in milliseconds
        // ミリ秒単位で指定された期間のバッファのサイズを返します。

        int __cdecl GetPendingBufferCount() const noexcept;

        const WAVEFORMATEX* __cdecl GetFormat() const noexcept;

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pImpl;
    };

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#endif;

    DEFINE_ENUM_FLAG_OPERATORS(AUDIO_ENGINE_FLAGS);
    DEFINE_ENUM_FLAG_OPERATORS(SOUND_EFFECT_INSTANCE_FLAGS);

#ifdef __clang__
#pragma clang diagnostic pop
#endif
}
