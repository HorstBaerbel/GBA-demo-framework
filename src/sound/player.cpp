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

constexpr int32_t VolumeDecPerFrame1s = VolumeMax / 64; // Volume reduced per frame for decay to 0 in 1s
constexpr int32_t VolumeDecPerFrame025s = VolumeMax / 16; // Volume reduced per frame for decay to 0 in 0.25s
constexpr mm_mixmode PlayerMixFrequency = MM_MIX_21KHZ; // Player mixxing frequency
constexpr uint32_t PlayerMixLength = MM_MIXLEN_21KHZ; // Length of player mixing buffer in bytes
constexpr uint32_t PlayerMixChannels = 8; // number of simultaneously mixed channels (MOD + effects)
constexpr uint32_t PlayerModChannels = 8; // number of simultaneously rendered channels in MOD

bool m_looping EWRAM_DATA = false;
uint32_t m_nrOfSongs EWRAM_DATA = 0;
int32_t m_currentSongNr EWRAM_DATA = -1;
mm_callback m_externalHandler EWRAM_DATA = nullptr;
int32_t m_volume_1s EWRAM_DATA = 0;
int32_t m_volume_025s EWRAM_DATA = 0;
int32_t m_bass EWRAM_DATA = 0;

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

int32_t getVolume_1s()
{
    return m_volume_1s;
}

int32_t getVolume_025s()
{
    return m_volume_025s;
}

bool isBass()
{
    return (m_bass > 0);
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
    if (songNr >= 0 && songNr < static_cast<int32_t>(m_nrOfSongs) && m_currentSongNr != songNr)
    {
        mmStop();
        m_currentSongNr = songNr;
        mmStart(songNr, MM_PLAY_ONCE);
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
    m_currentSongNr--;
    if (m_currentSongNr < 0)
    {
        m_currentSongNr = m_nrOfSongs - 1;
    }
    playSong(m_currentSongNr);
}

void skipNext()
{
    m_currentSongNr++;
    if (m_currentSongNr >= static_cast<int32_t>(m_nrOfSongs))
    {
        m_currentSongNr = 0;
    }
    playSong(m_currentSongNr);
}

void pause()
{
#ifdef DEBUG_PLAYER
    printf("Sound::pause()\n");
#endif
    mmPause();
}

void resume()
{
#ifdef DEBUG_PLAYER
    printf("Sound::resume()\n");
#endif
    mmResume();
}

void stop()
{
#ifdef DEBUG_PLAYER
    printf("Sound::stop()\n");
#endif
    mmStop();
    m_currentSongNr = -1;
}

//---mixing and callback------------------------------------------------------------

void volumeRecalc()
{
    // Bass is only valid for a couple of frames
    m_bass -= 1;
    m_bass = m_bass < 0 ? 0 : m_bass;
    // Decrease volume for frame and clamp it
    m_volume_1s -= VolumeDecPerFrame1s;
    m_volume_1s = m_volume_1s < 0 ? 0 : (m_volume_1s > VolumeMax ? VolumeMax : m_volume_1s);
    m_volume_025s -= VolumeDecPerFrame025s;
    m_volume_025s = m_volume_025s < 0 ? 0 : (m_volume_025s > VolumeMax ? VolumeMax : m_volume_025s);
}

unsigned int playerHandler(const unsigned int message, unsigned int parameter)
{
#ifdef DEBUG_PLAYER
    printf("Event 0x%x, Parameter 0x%x\n", (uint32_t)message, (uint32_t)parameter);
#endif
    // First process the messages we care for
    if (message == MMCB_SONGMESSAGE)
    {
        switch (parameter)
        {
            // Some SFx or EFx message. parameter is last nibble
            case 0xD:
                // not used here. can be used to switch scenes etc. when passed to external handler
                break;
            case 0xE:
                m_volume_1s += VolumeMax / 2;
                m_volume_025s += VolumeMax / 2;
                m_bass = 4;
                break;
            case 0xF:
                m_volume_1s += VolumeMax;
                m_volume_025s += VolumeMax;
                m_bass = 8;
                break;
            default:
                break;
        }
    }
    else if (message == MMCB_SONGFINISHED)
    {
        if (parameter == 0)
        {
            // Song message: main module finished
            m_looping ? playSong(m_currentSongNr) : skipNext();
        }
    }
    // Now call the external handler if set
    if (m_externalHandler)
    {
        return m_externalHandler(message, parameter);
    }
    return 0;
}

void setEventHandler(mm_word (*handler)(const mm_word message, mm_word parameter))
{
    m_externalHandler = handler;
}

void init(const void *soundbank, uint32_t nrOfSongs)
{
    m_nrOfSongs = nrOfSongs;
    // Give our vblank handler to maxmod, so it gets called after sound processing
    mmSetVBlankHandler(reinterpret_cast<void *>(Video::vblankHandler()));
    // We also need to call mmFrame every frame we display, so connect it to our handler
    // Will also call the playerHandler() if a song even occurrs
    Video::callAtVblank((void (*)())mmFrame);
    // After that connect handler to recalculate volume
    Video::callAtVblank((void (*)())volumeRecalc);
    // Maxmod requires the vblank interrupt to reset sound DMA. Link the VBlank interrupt to mmVBlank, and enable it.
    irqSet(IRQMask::IRQ_VBLANK, mmVBlank);
    irqEnable(IRQMask::IRQ_VBLANK);
    // Initialise maxmod with soundbank and 8 channels
    mmInitDefault((mm_addr)soundbank, PlayerModChannels);
    // Register our event handler
    mmSetEventHandler(playerHandler);
}

} //namespace Sound
