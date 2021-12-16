#include "player.h"
#include "memory/memory.h"
#include "sys/interrupts.h"
#include "video.h"

//#define DEBUG_PLAYER
#ifdef DEBUG_PLAYER
#include "print/print.h"
#endif

#ifdef CHECK_CPU_TIME
#include <gba_timers.h>
#endif

#include <maxmod.h>

namespace Sound
{

    constexpr mm_mixmode PlayerMixFrequency = MM_MIX_27KHZ; // Player mixxing frequency
    constexpr uint32_t PlayerMixLength = MM_MIXLEN_27KHZ;   // Length of player mixing buffer in bytes
    constexpr uint32_t PlayerMixChannels = 8;               // number of simultaneously mixed channels (MOD + effects)
    constexpr uint32_t PlayerModChannels = 8;               // number of simultaneously rendered channels in MOD

    LoopMode m_loopMode EWRAM_DATA = LoopMode::None;
    uint32_t m_nrOfSongs EWRAM_DATA = 0;
    int32_t m_currentSongNr EWRAM_DATA = -1;
#ifdef CHECK_CPU_TIME
    Math::fp1616_t m_cpuTime = 0;
#endif

    //--- song events -----------------------------------------------------------------------------

    constexpr uint32_t MaxSongEventFunctions = 6;
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

    uint32_t waveBufferLength()
    {
        return PlayerMixLength;
    }

    const uint8_t *getWaveBuffer()
    {
        return nullptr; // m_wave_buffer;
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
    }

#ifdef CHECK_CPU_TIME
    Math::fp1616_t getCpuTimeS()
    {
        return m_cpuTime;
    }

    void time_mmFrame()
    {
        REG_TM2CNT_L = 0;
        REG_TM2CNT_H = TIMER_START | 2;
        mmFrame();
        m_cpuTime = Math::fp1616_t::fromRaw(REG_TM2CNT_L);
        REG_TM2CNT_H = 0;
    }
#endif

    void init(const void *soundbank, uint32_t nrOfSongs)
    {
#ifdef DEBUG_PLAYER
        printf("Songs: %d\n", nrOfSongs);
#endif
        m_nrOfSongs = nrOfSongs;
        // Give our vblank handler to maxmod, so it gets called after sound processing
        mmSetVBlankHandler(reinterpret_cast<void *>(Video::vblankHandler()));
        // We also need to call mmFrame every frame we display, so connect it to our handler
#ifdef CHECK_CPU_TIME
        Video::callAtVblank((void (*)())time_mmFrame);
#else
        Video::callAtVblank((void (*)())mmFrame);
#endif
        // Maxmod requires the vblank interrupt to reset sound DMA. Link the VBlank interrupt to mmVBlank, and enable it.
        irqSet(IRQMask::IRQ_VBLANK, mmVBlank);
        irqEnable(IRQMask::IRQ_VBLANK);
        // Initialise maxmod with soundbank and 8 channels
        mmInitDefault((mm_addr)soundbank, PlayerModChannels);
        // Register our event handler to call if a song event occurrs
        mmSetEventHandler(eventHandler);
    }

} //namespace Sound
