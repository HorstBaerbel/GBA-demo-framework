#include "sound/moduleplayer.h"
#include "graphics.h"
#include "memory/memory.h"
#include "sys/interrupts.h"

// #define DEBUG_MODULEPLAYER
#ifdef DEBUG_MODULEPLAYER
#include "print/output.h"
#endif

// #define MODULEPLAYER_CPU_TIME
#ifdef MODULEPLAYER_CPU_TIME
#include "print/output.h"
#include "time.h"
#endif

#include <maxmod.h>

namespace ModulePlayer
{

    /// @brief Map mix frequency enum to frequency in Hz
    static const uint16_t MixFrequency2Hz[] = {
        8121,
        10512,
        13379,
        15768,
        18157,
        21024,
        26758,
        31536};

    /// @brief Map mix frequency to Maxmod mix buffer length
    static const uint16_t MixFrequency2MixLength[] = {
        MM_MIXLEN_8KHZ,
        MM_MIXLEN_10KHZ,
        MM_MIXLEN_13KHZ,
        MM_MIXLEN_16KHZ,
        MM_MIXLEN_18KHZ,
        MM_MIXLEN_21KHZ,
        MM_MIXLEN_27KHZ,
        MM_MIXLEN_31KHZ};

    IWRAM_DATA uint16_t m_nrOfSongs = 0;
    IWRAM_DATA int16_t m_currentSongNr = -1;
    IWRAM_DATA LoopMode m_loopMode = LoopMode::None;

    IWRAM_DATA uint16_t m_mixFrequencyHz = 21024;      // Player mixing frequency
    IWRAM_DATA uint16_t m_mixLength = MM_MIXLEN_21KHZ; // Length of player mixing buffer in bytes
    IWRAM_DATA uint16_t m_modChannels = 8;             // Number of simultaneously mixed channels (MOD + effects)
    IWRAM_DATA int8_t *m_mixingBuffer = nullptr;       // Pointer to mixing buffer
    IWRAM_DATA int8_t *m_modBuffer = nullptr;          // Pointer to MOD buffer
    IWRAM_DATA int8_t *m_waveBuffer = nullptr;         // Pointer to waveform buffer

    IWRAM_DATA Math::fp1616_t m_lastFrameCall = 0;
    IWRAM_DATA Math::fp1616_t m_playedDuration = 0;

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
#ifdef DEBUG_MODULEPLAYER
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
#ifdef DEBUG_MODULEPLAYER
        printf("Maxmod event 0x%x, parameter %d", static_cast<uint32_t>(message), static_cast<uint32_t>(parameter));
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
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::loopMode() = %d", static_cast<uint32_t>(m_loopMode));
#endif
    }

    uint32_t getMixFrequencyHz()
    {
        return m_mixFrequencyHz;
    }

    uint32_t getWaveBufferLength()
    {
        return m_mixLength;
    }

    const int8_t *getWaveBuffer()
    {
        return m_waveBuffer;
    }

    //---play control--------------------------------------------------------------

    Effect::Handle playEffect(const Effect *effect)
    {
        auto handle = mmEffectEx((mm_sound_effect *)effect);
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::playEffect(%d) -> Handle %d", effect->id, handle);
#endif
        return handle;
    }

    void stopEffect(Effect::Handle handle)
    {
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::stopEffect(%d)", handle);
#endif
        mmEffectCancel(handle);
    }

    void playSong(int32_t songNr)
    {
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::playSong(%d)", songNr);
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
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::setSongPosition(%d)", position);
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
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::pause(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmPause();
        songEvent(SongEvent::SongPaused, m_currentSongNr);
    }

    void resume()
    {
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::resume(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmResume();
        songEvent(SongEvent::SongResumed, m_currentSongNr);
    }

    void stop()
    {
#ifdef DEBUG_MODULEPLAYER
        printf("ModulePlayer::stop(), Module playing: ", static_cast<int32_t>(mmActive()));
#endif
        mmStop();
        songEvent(SongEvent::SongStopped, m_currentSongNr);
        m_playedDuration = 0;
    }

    IWRAM_FUNC void frame()
    {
#ifdef MODULEPLAYER_CPU_TIME
        auto startTime = Time::now();
#endif
        mmFrame();
        auto now = Math::fp1616_t::fromRaw(Time::now());
        if (mmActive())
        {
            m_playedDuration += now - m_lastFrameCall;
        }
        m_lastFrameCall = now;
#ifdef MODULEPLAYER_CPU_TIME
        static IWRAM_DATA int32_t sectionDuration = 0;
        static IWRAM_DATA int32_t sectionCount = 0;
        sectionDuration += Time::now() - startTime;
        sectionCount++;
        if (sectionDuration >= (2 << 16))
        {
            Debug::printf("ModulePlayer frame: %.2f", (sectionDuration * 1000) / sectionCount);
            sectionDuration = 0;
            sectionCount = 0;
        }
#endif
    }

    void init(const void *soundbank, uint32_t nrOfSongs, MixFrequency frequency, uint32_t channels)
    {
#ifdef DEBUG_MODULEPLAYER
        static const printf("Soundbank: 0x%x, Songs: %d, Mix frequency: %d, Channels: %d", soundbank, nrOfSongs, MixFrequency2Hz[static_cast<uint16_t>(frequency)], channels);
#endif
        m_nrOfSongs = nrOfSongs;
        m_mixFrequencyHz = MixFrequency2Hz[static_cast<uint16_t>(frequency)];
        m_modChannels = channels;
        // allocate buffers
        m_mixLength = MixFrequency2MixLength[static_cast<uint16_t>(frequency)];
        m_mixingBuffer = Memory::malloc_IWRAM<int8_t>(m_mixLength);
        m_modBuffer = Memory::malloc_IWRAM<int8_t>(m_modChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH) + m_mixLength);
        m_waveBuffer = m_modBuffer + m_modChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH);
        // Give our vblank handler to maxmod, so it gets called after sound processing
        mmSetVBlankHandler(reinterpret_cast<void *>(Graphics::vblankHandler()));
        // We also need to call mmFrame every frame we display, so connect it to our handler
        Graphics::callAtVblank((void (*)())frame);
        // Maxmod requires the vblank interrupt to reset sound DMA. Link the VBlank interrupt to mmVBlank, and enable it.
        Irq::setHandler(Irq::Mask::VBlank, mmVBlank);
        Irq::enable(Irq::Mask::VBlank);
        // Initialise Maxmod with soundbank
        mm_gba_system system;
        system.mixing_mode = static_cast<mm_mixmode>(frequency);
        system.mod_channel_count = m_modChannels;
        system.mix_channel_count = m_modChannels;
        system.module_channels = (mm_addr)m_modBuffer;
        system.active_channels = (mm_addr)(m_modBuffer + (m_modChannels * MM_SIZEOF_MODCH));
        system.mixing_channels = (mm_addr)(m_modBuffer + (m_modChannels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH)));
        system.mixing_memory = (mm_addr)m_mixingBuffer;
        system.wave_memory = (mm_addr)m_waveBuffer;
        system.soundbank = (mm_addr)soundbank;
        mmInit(&system);
        // Register our event handler to call if a song event occurrs
        mmSetEventHandler(eventHandler);
    }

    //--- spectrum -----------------------------------------------------------------------------

    void updateSpectrum()
    {
        Spectrum::update(getWaveBuffer(), getWaveBuffer() + m_mixLength / 2, m_mixLength / 2);
    }

    const Spectrum::Bands &getSpectrum()
    {
        return Spectrum::getSpectrum();
    }

} // namespace Sound
