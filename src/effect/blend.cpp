#include "blend.h"
#include "video.h"

namespace Effect_Blend
{

    /*inline Target operator|(Target a, Target b)
{
	return static_cast<Target>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}*/

    void procFade(void *data)
    {
        EffectData *currentData = (EffectData *)data;
        if (currentData)
        {
            // calculate elapsed time
            const Math::fp1616_t elapsedTime = Time::now() - currentData->startTime;
            // calculate state value t and clamp to [0,1]
            Math::fp1616_t t = clamp(elapsedTime / currentData->fadeTime, Math::fp1616_t(0), Math::fp1616_t(1));
            // calculate effect coefficient for target 1 in 1/16 steps and clamp to [0,16]
            uint16_t v1 = uint16_t(Math::fp1616_t(currentData->fromValue1) + t * (Math::fp1616_t(currentData->toValue1) - Math::fp1616_t(currentData->fromValue1)));
            v1 = v1 > 16 ? 16 : v1;
            // enable targets, and effect
            REG_BLDCNT = static_cast<uint16_t>(currentData->targets) | static_cast<uint16_t>(currentData->mode);
            // check if fading or alpha blending
            if (currentData->mode == Mode::MODE_ALPHA)
            {
                // calculate effect coefficient for target 2 in 1/16 steps and clamp to [0,16]
                uint16_t v2 = uint16_t(Math::fp1616_t(currentData->fromValue2) + t * (Math::fp1616_t(currentData->toValue2) - Math::fp1616_t(currentData->fromValue2)));
                v2 = v2 > 16 ? 16 : v2;
                // set value according to direction
                if (Direction::FADE_TO == currentData->direction || Direction::FADE_TO_AND_BACK == currentData->direction)
                {
                    REG_BLDALPHA = (v2 << 8) | v1;
                }
                else if (Direction::FADE_FROM == currentData->direction || Direction::FADE_FROM_AND_BACK == currentData->direction)
                {
                    REG_BLDALPHA = ((16 - v2) << 8) | (16 - v1);
                }
            }
            else
            {
                // set value according to direction
                if (Direction::FADE_TO == currentData->direction || Direction::FADE_TO_AND_BACK == currentData->direction)
                {
                    REG_BLDY = v1;
                }
                else if (Direction::FADE_FROM == currentData->direction || Direction::FADE_FROM_AND_BACK == currentData->direction)
                {
                    REG_BLDY = 16 - v1;
                }
            }
            // now check if we should stop or reverse fading
            if (elapsedTime >= currentData->fadeTime)
            {
                if (Direction::FADE_TO == currentData->direction || Direction::FADE_FROM == currentData->direction)
                {
                    // stop fading, thus remove callback from frame function
                    Video::removeAtVblank(nullptr, data);
                    clear();
                }
                else if (Direction::FADE_TO_AND_BACK == currentData->direction)
                {
                    // reverse direction
                    currentData->direction = Direction::FADE_FROM;
                    currentData->startTime = Time::now();
                }
                else if (Direction::FADE_FROM_AND_BACK == currentData->direction)
                {
                    // reverse direction
                    currentData->direction = Direction::FADE_TO;
                    currentData->startTime = Time::now();
                }
            }
        }
    }

    void startFade(EffectData &data)
    {
        //set up data
        data.startTime = Time::now();
        //connect effect to frame procedure
        Video::callAtVblank(procFade, (void *)&data);
    }

    void setFade(Target targets, Mode mode, Math::fp1616_t t)
    {
        if (mode == Mode::MODE_OFF)
        {
            clear();
        }
        else
        {
            // calculate effect coefficient for target 1 in 1/16 steps and clamp to [0,31]
            uint16_t v1 = uint16_t(t * Math::fp1616_t(16));
            v1 = v1 > 16 ? 16 : v1;
            // enable targets and effect and set value
            REG_BLDCNT = static_cast<uint16_t>(targets) | static_cast<uint16_t>(mode);
            REG_BLDY = v1;
        }
    }

    void setBlend(Target targets1, Target targets2, Math::fp1616_t t1, Math::fp1616_t t2)
    {
        // calculate effect coefficient for target 1 in 1/16 steps and clamp to [0,31]
        uint16_t v1 = uint16_t(t1 * Math::fp1616_t(16));
        v1 = v1 > 16 ? 16 : v1;
        // calculate effect coefficient for target 2 in 1/16 steps and clamp to [0,31]
        uint16_t v2 = uint16_t(t2 * Math::fp1616_t(16));
        v2 = v2 > 16 ? 16 : v2;
        // enable targets and effect and set values
        REG_BLDCNT = static_cast<uint16_t>(targets1) | static_cast<uint16_t>(targets2) | static_cast<uint16_t>(Mode::MODE_ALPHA);
        REG_BLDY = (v2 << 8) | v1;
    }

    void clear()
    {
        Video::removeAtVblank(procFade, nullptr);
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
    }

} // namespace Effect_Blend
