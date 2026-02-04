#pragma once

#include <cstdint>

namespace Time
{

	/// @brief Enable timer. now() value increases every ~1ms now.
	auto start() -> void;

	/// @brief Stop timer. now() value will not increase anymore.
	auto stop() -> void;

	/// @brief Return time since timer was started.
	/// @return Returns the current time in seconds in 16:16 format. Use Math::fp1616_t::fromRaw() to convert to fixed-point
	/// @note This will wrap around ~4,5 hours after start() was called.
	auto now() -> int32_t;

} // namespace Time
