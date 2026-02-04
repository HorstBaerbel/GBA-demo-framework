#include "fadepalette.h"
#include "graphics.h"

namespace Effect_FadePalette
{

	void procFade(void *data)
	{
		EffectData *currentData = (EffectData *)data;
		if (currentData)
		{
			// calculate elapsed time
			Math::fp1616_t elapsedTime = Math::fp1616_t::fromRaw(Time::now()) - currentData->startTime;
			// calculate state value t and clamp to [0,1]
			Math::fp1616_t t = clamp(elapsedTime / currentData->fadeTime, Math::fp1616_t(0), Math::fp1616_t(1));
			// set up pointers to palettes
			const color16 *from = (Mode::FADE_TO == currentData->mode || Mode::FADE_TO_AND_BACK == currentData->mode) ? currentData->from : currentData->to;
			const color16 *to = (Mode::FADE_TO == currentData->mode || Mode::FADE_TO_AND_BACK == currentData->mode) ? currentData->to : currentData->from;
			// loop through palette colors.
			for (int i = 0; i < 256; i++)
			{
				// get fade source color
				int16_t rF = from[i] & 0x1F;
				int16_t gF = (from[i] >> 5) & 0x1F;
				int16_t bF = (from[i] >> 10) & 0x1F;
				// get fade target color
				int16_t rT = to[i] & 0x1F;
				int16_t gT = (to[i] >> 5) & 0x1F;
				int16_t bT = (to[i] >> 10) & 0x1F;
				// a * (1-t) + bt = a - at + bt = a + -at + bt = a + (b-a)t
				rF += int16_t(t * (rT - rF));
				gF += int16_t(t * (gT - gF));
				bF += int16_t(t * (bT - bF));
				// write color to palette memory
				Palette::Background[i] = (bF << 10) | (gF << 5) | rF;
			}
			// now check if we should stop or reverse fading
			if (elapsedTime >= currentData->fadeTime)
			{
				if (Mode::FADE_TO == currentData->mode || Mode::FADE_FROM == currentData->mode)
				{
					// stop fading, thus remove callback from frame function
					Graphics::removeAtVblank(nullptr, data);
				}
				else if (Mode::FADE_TO_AND_BACK == currentData->mode)
				{
					// reverse fading
					currentData->mode = Mode::FADE_FROM;
					currentData->startTime = Math::fp1616_t::fromRaw(Time::now());
				}
				else if (Mode::FADE_FROM_AND_BACK == currentData->mode)
				{
					// reverse fading
					currentData->mode = Mode::FADE_TO;
					currentData->startTime = Math::fp1616_t::fromRaw(Time::now());
				}
			}
		}
	}

	void start(EffectData &fadeData)
	{
		//set up time
		fadeData.startTime = Math::fp1616_t::fromRaw(Time::now());
		//connect effect to frame procedure
		Graphics::callAtVblank(procFade, (void *)&fadeData);
	}

	void clear()
	{
		Graphics::removeAtVblank(procFade);
	}

} //namespace Effect_FadePalette
