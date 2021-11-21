#include "player.h"
#include "memory/memory.h"
#include "sys/interrupts.h"
#include "video.h"

#ifdef DEBUG_PLAYER
#include "debug/print.h"
#endif

#include <maxmod.h>

namespace Sound
{

    constexpr mm_mixmode PlayerMixFrequency = MM_MIX_21KHZ; // Player mixxing frequency
    constexpr uint32_t PlayerMixLength = MM_MIXLEN_21KHZ;   // Length of player mixing buffer in bytes
    constexpr uint32_t PlayerMixChannels = 8;               // number of simultaneously mixed channels (MOD + effects)
    constexpr uint32_t PlayerModChannels = 8;               // number of simultaneously rendered channels in MOD

    bool m_looping EWRAM_DATA = false;
    uint32_t m_nrOfSongs EWRAM_DATA = 0;
    int32_t m_currentSongNr EWRAM_DATA = -1;

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

    void songEvent(const SongEvent &event)
    {
        for (uint32_t i = 0; i < m_nrOfSongEventFunctions; i++)
        {
            auto &func = m_songEventFunctions[i];
            if (func != nullptr)
            {
                func(event);
            }
        }
    }

    unsigned int eventHandler(const unsigned int message, unsigned int parameter)
    {
#ifdef DEBUG_PLAYER
        printf("Event 0x%x, Parameter 0x%x\n", (uint32_t)message, (uint32_t)parameter);
#endif
        if (message == MMCB_SONGMESSAGE)
        {
            // Song message from EFx / SFx effect
            songEvent({SongEvent::Type::SongMessage, static_cast<int16_t>(parameter)});
        }
        else if (message == MMCB_SONGFINISHED)
        {
            if (parameter == 0)
            {
                // Song message: main module finished
                m_looping ? playSong(m_currentSongNr) : skipNext();
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

    bool isLooping()
    {
        return m_looping;
    }

    void setLooping(bool loop)
    {
        m_looping = loop;
#ifdef DEBUG_PLAYER
        printf("Sound::isLooping() = %d\n", m_looping);
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
        printf("Sound::playEffect(%d) -> Handle %d\n", effect.id, handle);
#endif
        return handle;
    }

    void stopEffect(Effect::Handle handle)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::stopEffect(%d)\n", handle);
#endif
        mmEffectCancel(handle);
    }

    void playSong(int32_t songNr)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::play(%d)\n", songNr);
#endif
        if (songNr >= 0 && songNr < static_cast<int32_t>(m_nrOfSongs))
        {
            if (mmActive() != 0)
            {
                mmStop();
                songEvent({SongEvent::Type::SongStopped, static_cast<int16_t>(m_currentSongNr)});
            }
            m_currentSongNr = songNr;
            mmStart(songNr, MM_PLAY_ONCE);
            songEvent({SongEvent::Type::SongStarted, static_cast<int16_t>(m_currentSongNr)});
        }
    }

    void setSongPosition(uint32_t position)
    {
#ifdef DEBUG_PLAYER
        printf("Sound::setSongPosition(%d)\n", position);
#endif
        mmPosition(position);
    }

    void skipPrevious()
    {
        auto newSongNr = m_currentSongNr - 1;
        if (newSongNr < 0)
        {
            newSongNr = m_nrOfSongs - 1;
        }
        playSong(newSongNr);
    }

    void skipNext()
    {
        auto newSongNr = m_currentSongNr + 1;
        if (newSongNr >= static_cast<int32_t>(m_nrOfSongs))
        {
            newSongNr = 0;
        }
        playSong(newSongNr);
    }

    void pause()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::pause()\n");
#endif
        if (mmActive() != 0)
        {
            mmPause();
            songEvent({SongEvent::Type::SongPaused, static_cast<int16_t>(m_currentSongNr)});
        }
    }

    void resume()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::resume()\n");
#endif
        mmResume();
        if (mmActive() != 0)
        {
            songEvent({SongEvent::Type::SongStarted, static_cast<int16_t>(m_currentSongNr)});
        }
    }

    void stop()
    {
#ifdef DEBUG_PLAYER
        printf("Sound::stop()\n");
#endif
        if (mmActive() != 0)
        {
            songEvent({SongEvent::Type::SongStopped, static_cast<int16_t>(m_currentSongNr)});
            mmStop();
            m_currentSongNr = -1;
        }
    }

    void init(const void *soundbank, uint32_t nrOfSongs)
    {
        m_nrOfSongs = nrOfSongs;
        // Give our vblank handler to maxmod, so it gets called after sound processing
        mmSetVBlankHandler(reinterpret_cast<void *>(Video::vblankHandler()));
        // We also need to call mmFrame every frame we display, so connect it to our handler
        // Will also call the playerHandler() if a song even occurrs
        Video::callAtVblank((void (*)())mmFrame);
        // Maxmod requires the vblank interrupt to reset sound DMA. Link the VBlank interrupt to mmVBlank, and enable it.
        irqSet(IRQMask::IRQ_VBLANK, mmVBlank);
        irqEnable(IRQMask::IRQ_VBLANK);
        // Initialise maxmod with soundbank and 8 channels
        mmInitDefault((mm_addr)soundbank, PlayerModChannels);
        // Register our event handler
        mmSetEventHandler(eventHandler);
    }

} //namespace Sound
