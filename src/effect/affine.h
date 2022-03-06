#pragma once

#include "fptime.h"
#include "math/fp32.h"

namespace Effect_Affine
{

	//-----BG2/3 scale effect------------------------------------------------------
	// See: http://problemkaputt.de/gbatek.htm#lcdiobgrotationscaling

	enum class Target : uint16_t
	{
		TARGET_BG2, /// Background 2.
		TARGET_BG3	/// Background 3.
	};

	struct AffineData
	{
		int16_t dx;	 // PA / m11 (s19.8 format)
		int16_t dmx; // PB / m12 (s19.8 format)
		int16_t dy;	 // PC / m21 (s19.8 format)
		int16_t dmy; // PD / m22 (s19.8 format)
		Math::fp1616_t refx;
		Math::fp1616_t refy;
	} __attribute__((aligned(4), packed));

	struct EffectData
	{
		Target target;
		uint16_t dummy;
		AffineData affine;
	} __attribute__((aligned(4), packed));

	/// @brief Create identity transform.
	AffineData createIdentity();
	/// @brief Create scale from values. angle is in radians.
	AffineData createScale(Math::fp1616_t refx, Math::fp1616_t refy, Math::fp1616_t angle, Math::fp1616_t sx = 1, Math::fp1616_t sy = 1);
	/// @brief Create centered scale data from values. angle is in radians.
	AffineData createScaleCenter(Math::fp1616_t angle, Math::fp1616_t sx = 1, Math::fp1616_t sy = 1);
	/// @brief Create mirror transform that mirrors the screen horizontally and flips the bottom part vertically.
	AffineData createMirrorHFlipV(uint16_t startLine = 79, uint16_t screenWidth = 240);

	/// @brief Set data from struct to hardware.
	void setData(Target target, const AffineData &data);
	/// @brief Set data from struct to hardware.
	void setData(const EffectData &data);

	/// @brief Screen effect that shifts BG2 a random amount of pixes left/right.
	/// @param baseShift Start value before and after shifting occurrs. Random values will be added on top.
	/// @param pixelsLog2 log2(pixels) to shift the screen.
	void shiftScreenLinesRandom(Target target, Math::fp1616_t baseShift = 0, uint8_t pixelsLog2 = 3);

	/// @brief Screen effect that shifts BG2 a random amount of pixes left/right.
	/// @param baseShift Start value before and after shifting occurrs. Random values will be added on top.
	/// @param scale Scale factor for sine.
	/// @param amplitude Amplitude of sine.
	/// @param phaseInc Value to add to phase after each call of the function.
	void shiftScreenLinesSine(Target target, Math::fp1616_t baseShift = 0, Math::fp1616_t scale = 2, Math::fp1616_t amplitude = 3, Math::fp1616_t phaseInc = 0.9f);

	/// @brief Clear all registers to their default values, turning off all effects.
	void clear();

} // namespace Effect_Affine