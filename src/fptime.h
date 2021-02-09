#pragma once

#include "math/fp32.h"

namespace Time
{

	/// @brief Enable timer. getTime() value increases every ~5ms now.
	void start();

	/// @brief Stop timer. getTime() value will not increase anymore.
	void stop();

	/// @brief Return time since timer was started.
	/// @return Returns the current time in seconds.
	/// @note This will wrap around ~9,1 hours after start() was called.
	Math::fp1616_t getTime();

	/// @brief Set time to new value.
	/// @param[in] time New time value.
	void setTime(Math::fp1616_t time);

	/// @brief Set time scale. Use this to make time run faster/slower or backwards.
	/// @param[in] scale Scaling value that gets multiplied when timerTick() is called.
	void setScale(Math::fp1616_t scale);

} // namespace Time
