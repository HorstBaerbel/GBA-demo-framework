#include "sound/sound.h"
#include "graphics.h"
#include "memory/memory.h"
#include "sys/interrupts.h"

//#define DEBUG_PLAYER
#ifdef DEBUG_PLAYER
#include "print/print.h"
#endif

#ifdef CHECK_CPU_TIME
#include "fptime.h"
#endif

#include <maxmod.h>

namespace Sound
{

    constexpr mm_mixmode PlayerMixFrequency = MM_MIX_21KHZ; // Player mixxing frequency
    constexpr uint32_t PlayerMixLength = MM_MIXLEN_21KHZ;   // Length of player mixing buffer in bytes
    constexpr uint32_t PlayerMixChannels = 8;               // number of simultaneously mixed channels (MOD + effects)
    constexpr uint32_t PlayerModChannels = 8;               // number of simultaneously rendered channels in MOD

    IWRAM_DATA ALIGN(4) int8_t m_mixingBuffer[PlayerMixLength];
    IWRAM_DATA ALIGN(4) int8_t m_maxmodBuffer[PlayerModChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH) + PlayerMixLength];

    EWRAM_DATA LoopMode m_loopMode = LoopMode::None;
    EWRAM_DATA uint32_t m_nrOfSongs = 0;
    EWRAM_DATA int32_t m_currentSongNr = -1;
    IWRAM_DATA Time::TimePoint m_lastFrameCall = 0;
    IWRAM_DATA Time::Duration m_playedDuration = 0;

#ifdef CHECK_CPU_TIME
    Math::fp1616_t m_cpuTime = 0;

    Math::fp1616_t getCpuTimeS()
    {
        return m_cpuTime;
    }
#endif

    //--- song events -----------------------------------------------------------------------------

    constexpr uint32_t MaxSongEventFunctions = 4;
    SongEventHandler m_songEventFunctions[MaxSongEventFunctions];
    uint32_t m_nrOfSongEventFunctions = 0;

    void callAtSongEvent(SongEventHandler handler)
    {
        if (m_nrOfSongEventFunctions < MaxSongEventFunctions)
        {
            m_songEventFunctions[m_nrOfSongEventFunctions] = handler;
            m_nrOfSongEventFunctions++;
        }
    }

    void removeAtSongEvent(SongEventHandler handler)
    {
        for (int32_t i = 0; i < static_cast<int32_t>(m_nrOfSongEventFunctions); i++)
        {
            if (handler != nullptr && handler == m_songEventFunctions[i])
            {
                // move all entries one to the front
                for (; i < static_cast<int32_t>(m_nrOfSongEventFunctions) - 1; i++)
                {
                    m_songEventFunctions[i] = m_songEventFunctions[i + 1];
                }
                m_nrOfSongEventFunctions--;
                break;
            }
        }
    }

    void songEvent(SongEvent event, int32_t parameter)
    {
#ifdef DEBUG_PLAYER
        printf("Sending song event %d, parameter %d", static_cast<uint32_t>(event), parameter);
#endif
        for (uint32_t i = 0; i < m_nrOfSongEventFunctions; i++)
        {
            auto &func = m_songEventFunctions[i];
            if (func != nullptr)
            {
                func(event, parameter);
            }
        }
    }

    unsigned int eventHandler(const unsigned int message, unsigned int parameter)
    {
#ifdef DEBUG_PLAYER
        printf("MaxMod event 0x%x, parameter %d", static_cast<uint32_t>(message), static_cast<uint32_t>(parameter));
#endif
        if (message == MMCB_SONGMESSAGE)
        {
            // Song message from EFx / SFx effect
            songEvent(SongEvent::SongMessage, parameter);
        }
        else if (message == MMCB_SONGFINISHED)
        {
            // Song message: Main module finished
            // necessary? if (parameter == 0)
            switch (m_loopMode)
            {
            case LoopMode::LoopOne:
                playSong(m_currentSongNr);
                break;
            case LoopMode::LoopAll:
                skipNext();
                break;
            default:
                songEvent(SongEvent::SongStopped, m_currentSongNr);
                break;
            }
        }
        return 0;
    }

    //---get/set-------------------------------------------------------------------

    uint32_t getSongCount()
    {
        return m_nrOfSongs;
    }

    int32_t getCurrentSongNr()
    {
        return m_currentSongNr;
    }

    LoopMode getLoopMode()
    {
        return m_loopMode;
    }

    void setLoopMode(LoopMode mode)
    {
        m_loopMode = mode;
#ifdef DEBUG_PLAYER
        printf("Sound::loopMode() = %d", static_cast<uint32_t>(m_loopMode));
#endif
    }

    constexpr uint32_t getWaveBufferLength()
    {
        return PlayerMixLength;
    }

    const int8_t *getWaveBuffer()
    {
        return m_maxmodBuffer + (PlayerModChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH));
    }

    //---play control--------------------------------------------------------------

    Effect::Handle playEffect(const Effect *effect)
    {
        auto handle = mmEffectEx((mm_sound_effect *)effect);
#ifdef DEBUG_PLAYER
        printf("Sound::playEffect(%d) -> Handle %d", effect->id, handle);
#endif
        return handle;
    }

    void stopEffect(Effect::Handle handle)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::stopEffect(%d)", handle);
#endif
        mmEffectCancel(handle);
    }

    void playSong(int32_t songNr)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::play(%d)", songNr);
#endif
        if (songNr >= 0 && songNr < static_cast<int32_t>(m_nrOfSongs))
        {
            if (mmActive())
            {
                mmStop();
            }
            m_currentSongNr = songNr;
            m_playedDuration = 0;
            mmStart(songNr, MM_PLAY_ONCE);
            songEvent(SongEvent::SongStarted, m_currentSongNr);
        }
    }

    void setSongPosition(uint32_t position)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::setSongPosition(%d)", position);
#endif
        mmPosition(position);
    }

    Math::fp1616_t getPlayedSongDuration()
    {
        return m_playedDuration;
    }

    void skipPrevious()
    {
        auto newSongNr = m_currentSongNr >= 0 ? m_currentSongNr - 1 : 0;
        if (newSongNr < 0)
        {
            newSongNr = m_nrOfSongs - 1;
        }
        playSong(newSongNr);
    }

    void skipNext()
    {
        auto newSongNr = m_currentSongNr >= 0 ? m_currentSongNr + 1 : 0;
        if (newSongNr >= static_cast<int32_t>(m_nrOfSongs))
        {
            newSongNr = 0;
        }
        playSong(newSongNr);
    }

    void pause()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::pause(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmPause();
        songEvent(SongEvent::SongPaused, m_currentSongNr);
    }

    void resume()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::resume(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmResume();
        songEvent(SongEvent::SongResumed, m_currentSongNr);
    }

    void stop()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::stop(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmStop();
        songEvent(SongEvent::SongStopped, m_currentSongNr);
        m_playedDuration = 0;
    }

    IWRAM_FUNC void frame()
    {
#ifdef CHECK_CPU_TIME
        auto startTime = Time::now();
#endif
        mmFrame();
        auto now = Time::now();
        if (mmActive())
        {
            m_playedDuration += now - m_lastFrameCall;
        }
        m_lastFrameCall = now;
#ifdef CHECK_CPU_TIME
        m_cpuTime = now - startTime;
#endif
    }

    void init(const void *soundbank, uint32_t nrOfSongs)
    {
#ifdef DEBUG_PLAYER
        printf("Songs: %d\n", nrOfSongs);
#endif
        m_nrOfSongs = nrOfSongs;
        // Give our vblank handler to maxmod, so it gets called after sound processing
        mmSetVBlankHandler(reinterpret_cast<void *>(Graphics::vblankHandler()));
        // We also need to call mmFrame every frame we display, so connect it to our handler
        Graphics::callAtVblank((void (*)())frame);
        // Maxmod requires the vblank interrupt to reset sound DMA. Link the VBlank interrupt to mmVBlank, and enable it.
        irqSet(IRQMask::IRQ_VBLANK, mmVBlank);
        irqEnable(IRQMask::IRQ_VBLANK);
        // Initialise maxmod with soundbank and 8 channels
        //mmInitDefault((mm_addr)soundbank, PlayerModChannels);
        mm_gba_system system;
        system.mixing_mode = PlayerMixFrequency;
        system.mod_channel_count = PlayerModChannels;
        system.mix_channel_count = PlayerModChannels;
        system.module_channels = (mm_addr)(m_maxmodBuffer + 0);
        system.active_channels = (mm_addr)(m_maxmodBuffer + (PlayerModChannels * MM_SIZEOF_MODCH));
        system.mixing_channels = (mm_addr)(m_maxmodBuffer + (PlayerModChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH)));
        system.mixing_memory = (mm_addr)m_mixingBuffer;
        system.wave_memory = (mm_addr)(m_maxmodBuffer + (PlayerModChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH)));
        system.soundbank = (mm_addr)soundbank;
        mmInit(&system);
        // Register our event handler to call if a song event occurrs
        mmSetEventHandler(eventHandler);
    }

    //--- spectrum -----------------------------------------------------------------------------

    void updateSpectrum()
    {
        Spectrum::update(getWaveBuffer(), getWaveBuffer() + PlayerMixLength / 2, PlayerMixLength / 2);
    }

    const Spectrum::Bands &getSpectrum()
    {
        return Spectrum::getSpectrum();
    }

} //namespace Sound
