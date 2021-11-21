#include "volume.h"
#include "video.h"

#ifdef DEBUG_PLAYER
#include "debug/print.h"
#endif

namespace Volume
{

    constexpr int32_t VolumeDecPerFrame1s = VolumeMax / 64;   // Volume reduced per frame for decay to 0 in 1s
    constexpr int32_t VolumeDecPerFrame025s = VolumeMax / 16; // Volume reduced per frame for decay to 0 in 0.25s

    int32_t m_volume_1s EWRAM_DATA = 0;
    int32_t m_volume_025s EWRAM_DATA = 0;
    int32_t m_bass EWRAM_DATA = 0;

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

    void handleSongEvent(const Sound::SongEvent &event)
    {
        // First process the messages we care about
        if (event.type == Sound::SongEvent::Type::SongMessage)
        {
            switch (event.parameter)
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
    }

    void init()
    {
        // Connect handler to recalculate volume on vblank
        Video::callAtVblank((void (*)())volumeRecalc);
    }

} //namespace Sound
