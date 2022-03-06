#include "affine.h"

#include "math/random.h"
#include "graphics.h"

namespace Effect_Affine
{

    //-----BG2/3 scale effect------------------------------------------------------
    // See: http://problemkaputt.de/gbatek.htm#lcdiobgrotationscaling

    Math::fp1616_t phase = Math::fp1616_t::ZERO;

    AffineData createIdentity()
    {
        AffineData result;
        result.dx = 256;
        result.dmx = 0;
        result.dy = 0;
        result.dmy = 256;
        result.refx = 0;
        result.refy = 0;
        return result;
    }

    AffineData createScale(Math::fp1616_t refx, Math::fp1616_t refy, Math::fp1616_t angle, Math::fp1616_t sx, Math::fp1616_t sy)
    {
        AffineData result;
        Math::fp1616_t srad(0);
        Math::fp1616_t crad(1);
        if (angle != 0)
        {
            sincos(angle, srad, crad);
        }
        result.dx = (int16_t)((crad / sx).raw() >> 8);
        result.dmx = (int16_t)((-srad / sx).raw() >> 8);
        result.dy = (int16_t)((srad / sy).raw() >> 8);
        result.dmy = (int16_t)((crad / sy).raw() >> 8);
        result.refx = refx;
        result.refy = refy;
        return result;
    }

    AffineData createScaleCenter(Math::fp1616_t rad, Math::fp1616_t sx, Math::fp1616_t sy)
    {
        AffineData result;
        Math::fp1616_t srad(0);
        Math::fp1616_t crad(1);
        if (rad != 0)
        {
            sincos(rad, srad, crad);
        }
        Math::fp1616_t pa = crad / sx;
        Math::fp1616_t pb = -srad / sx;
        Math::fp1616_t pc = srad / sy;
        Math::fp1616_t pd = crad / sy;
        // set rotation
        result.dx = (int16_t)(pa.raw() >> 8);
        result.dmx = (int16_t)(pb.raw() >> 8);
        result.dy = (int16_t)(pc.raw() >> 8);
        result.dmy = (int16_t)(pd.raw() >> 8);
        // make sure we rotate around the center
        result.refx = Math::fp1616_t(120) - (pa * Math::fp1616_t(120) + pb * Math::fp1616_t(80));
        result.refy = Math::fp1616_t(80) - (pc * Math::fp1616_t(120) + pd * Math::fp1616_t(80));
        return result;
    }

    AffineData createMirrorHFlipV(uint16_t startLine, uint16_t screenWidth)
    {
        AffineData result;
        result.dx = -256;
        result.dmx = 0;
        result.dy = 0;
        result.dmy = -256;
        result.refx = Math::fp1616_t::fromRaw(((screenWidth << 8) - 1) << 8);
        result.refy = Math::fp1616_t::fromRaw(((startLine << 8) - 1) << 8);
        return result;
    }

    void setData(Target target, const AffineData &data)
    {
        if (target == Target::TARGET_BG2)
        {
            REG_BG2PA = data.dx;
            REG_BG2PB = data.dmx;
            REG_BG2PC = data.dy;
            REG_BG2PD = data.dmy;
            REG_BG2X = data.refx.raw() >> 8;
            REG_BG2Y = data.refy.raw() >> 8;
        }
        else if (target == Target::TARGET_BG3)
        {
            REG_BG3PA = data.dx;
            REG_BG3PB = data.dmx;
            REG_BG3PC = data.dy;
            REG_BG3PD = data.dmy;
            REG_BG3X = data.refx.raw() >> 8;
            REG_BG3Y = data.refy.raw() >> 8;
        }
    }

    void setData(const EffectData &data)
    {
        setData(data.target, data.affine);
    }

    void shiftScreenLinesRandom(Target target, Math::fp1616_t baseShift, uint8_t pixelsLog2)
    {
        int32_t &REG_TARGET = (target == Target::TARGET_BG2 ? REG_BG2X : REG_BG3X);
        // set base position
        REG_TARGET = baseShift.raw() >> 8;
        // wait while in vblank
        while (REG_VCOUNT >= 160)
        {
        }
        uint8_t vcount = 255;
        while (REG_VCOUNT < 160)
        {
            // wait for the line to change
            if (vcount != REG_VCOUNT)
            {
                // shift horizontal start of screen
                vcount = REG_VCOUNT;
                int32_t shift = ((int32_t)(random<uint32_t>() >> (31 - pixelsLog2))) - ((int32_t)1 << pixelsLog2);
                REG_TARGET = (baseShift.raw() >> 8) + (shift << 8);
            }
        }
        // set base position again
        REG_TARGET = baseShift.raw() >> 8;
    }

    void shiftScreenLinesSine(Target target, Math::fp1616_t baseShift, Math::fp1616_t scale, Math::fp1616_t amplitude, Math::fp1616_t phaseInc)
    {
        int32_t &REG_TARGET = (target == Target::TARGET_BG2 ? REG_BG2X : REG_BG3X);
        // set base position
        REG_TARGET = baseShift.raw() >> 8;
        // wait while in vblank
        while (REG_VCOUNT >= 160)
        {
        }
        // now wait until display has started
        uint8_t vcount = 255;
        while (REG_VCOUNT < 160)
        {
            // wait for the line to change
            if (vcount != REG_VCOUNT)
            {
                // shift horizontal start of screen
                vcount = REG_VCOUNT;
                Math::fp1616_t sineFactor = cos(scale * (phase + vcount) / Math::fp1616_t(160.0f) * Math::fp1616_t::PI_2);
                REG_TARGET = (baseShift.raw() >> 8) + ((sineFactor * amplitude).raw() >> 8);
                //Effect_BG::setFade(Effect_BG::FadeTarget::TARGET_BLACK, (sineFactor + 1.0f) / 8.0f);
            }
        }
        // set base position
        REG_TARGET = baseShift.raw() >> 8;
        phase += phaseInc;
    }

    void clear()
    {
        REG_BG2PA = 256;
        REG_BG2PB = 0;
        REG_BG2PC = 0;
        REG_BG2PD = 256;
        REG_BG2X = 0;
        REG_BG2Y = 0;
        REG_BG3PA = 256;
        REG_BG3PB = 0;
        REG_BG3PC = 0;
        REG_BG3PD = 256;
        REG_BG3X = 0;
        REG_BG3Y = 0;
    }

} //namespace Effect_Affine
