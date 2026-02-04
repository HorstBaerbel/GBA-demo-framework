#pragma once

#include "time.h"
#include "color.h"
#include "palette.h"

namespace Effect_FadePalette
{

	enum class Mode : uint8_t
	{
		FADE_TO,		   /// Fade from current palette to "to" palette.
		FADE_TO_AND_BACK,  /// Fade from current palette to "to" palette and back.
		FADE_FROM,		   /// Fade from "from" palette to "to" palette.
		FADE_FROM_AND_BACK /// Fade from "from" palette to "to" palette and back.
	};

	struct EffectData
	{
		Palette::Palette256 from; /// Palette to fade from. color 0 is not used.
		Palette::Palette256 to;	  /// Palette to fade to. color 0 is not used.
		Math::fp1616_t fadeTime;  /// The time the fade to or fade from operation should take.
		Math::fp1616_t startTime; /// Time the effect was started.
		Mode mode;				  /// Fade mode used.
	} __attribute__((aligned(4), packed));

	void start(EffectData &fadeData);

	void clear();

} // namespace Effect_FadePalette
