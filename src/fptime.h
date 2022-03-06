#pragma once

#include "math/fp32.h"

namespace Time
{

	/// @brief Time point in seconds e.g. from now()
	using TimePoint = Math::fp1616_t;

	/// @brief Duration in seconds
	using Duration = Math::fp1616_t;

	/// @brief Enable timer. now() value increases every ~2.5ms now.
	void start();

	/// @brief Stop timer. now() value will not increase anymore.
	void stop();

	/// @brief Return time since timer was started.
	/// @return Returns the current time in seconds.
	/// @note This will wrap around ~4,5 hours after start() was called.
	TimePoint now();

	/// @brief Set time to new value.
	/// @param[in] time New time value.
	void setNow(TimePoint time);

	/// @brief Set time scale. Use this to make time run faster/slower or backwards.
	/// @param[in] scale Scaling value that gets multiplied when timerTick() is called.
	void setScale(Math::fp1616_t scale);

	/// @brief Start a short-term timer (< 1s). For benchmarking etc.
	/// Runs on Timer #2.
	void startTimer();

	/// @brief Stop short-term timer and return result in s.
	/// Runs on Timer #2.
	Duration endTimer();

} // namespace Time
