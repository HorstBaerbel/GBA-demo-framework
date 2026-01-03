#pragma once

#include "sys/base.h"

#include <cstdint>

namespace Memory
{

	/// @brief Register for Game Pak SRAM and ROM wait states
	inline auto &RegWaitCnt{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0204)};

	/// @brief Minimum wait states possible for Game Pak SRAM and ROM (2,1 + prefetch enabled)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint16_t WaitCntFast = 0x46DA;

	/// @brief Regular wait states possible for Game Pak SRAM and ROM (3,1 + prefetch enabled)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint16_t WaitCntNormal = 0x4317;

	/// @brief Regular wait states possible for Game Pak SRAM and ROM (4,2 + prefetch enabled)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint16_t WaitCntDefault = 0x4000;

	/// @brief Check if the wait state value for ROM can be safely used.
	/// If so sets that value, else falls back to WaitCntDefault.
	/// @param value Wait state value
	/// @return Returns true if the vale can be used, false otherwise.
	auto trySetWaitCnt(uint16_t value) -> bool;

	/// @brief Register for EWRAM wait states
	inline auto &RegWaitEwram{*reinterpret_cast<volatile uint32_t *>(REG_BASE + 0x0800)};

	/// @brief Wait states for EWRAM that crash the GBA (1/1/2)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint32_t WaitEwramLudicrous = 0x0F000020;

	/// @brief Minimum wait states possible for EWRAM (2/2/4)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint32_t WaitEwramFast = 0x0E000020;

	/// @brief Regular wait states possible for EWRAM (3/3/6)
	/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
	constexpr uint32_t WaitEwramNormal = 0x0D000020;

	/// @brief Allocate a block of IWRAM.
	/// @return Returns a pointer to the memory or NULL if allocation failed.
	void *malloc_IWRAM(uint32_t nrOfBytes);

	/// @brief Allocate a block of IWRAM of specific type.
	/// @param nrOfT Number of Ts to allocate.
	/// @return Returns a pointer to the memory or NULL if allocation failed.
	template <typename T>
	T *malloc_IWRAM(uint32_t nrOfT) { return reinterpret_cast<T *>(malloc_IWRAM(sizeof(T) * nrOfT)); }

	/// @brief Allocate a block of EWRAM.
	/// @return Returns a pointer to the memory or NULL if allocation failed.
	void *malloc_EWRAM(uint32_t nrOfBytes);

	/// @brief Allocate a block of EWRAM of specific type.
	/// @param nrOfT Number of Ts to allocate.
	/// @return Returns a pointer to the memory or NULL if allocation failed.
	template <typename T>
	T *malloc_EWRAM(uint32_t nrOfT) { return reinterpret_cast<T *>(malloc_EWRAM(sizeof(T) * nrOfT)); }

	/// @brief Free IWRAM or EWRAM memory.
	void free(void *adress);

	/// @brief Free IWRAM or EWRAM memory.
	void free(const void *adress);

	/// @brief Copy words from source to destination.
	/// @param destination Copy destination.
	/// @param source Copy source.
	/// @param nrOfHwords Number of half-words to copy.
	extern "C" void memcpy16(void *destination, const void *source, uint32_t nrOfHwords);

	/// @brief Copy words from source to destination.
	/// @param destination Copy destination.
	/// @param source Copy source.
	/// @param nrOfWords Number of words to copy.
	extern "C" void memcpy32(void *destination, const void *source, uint32_t nrOfWords) IWRAM_FUNC;

	/// @brief Set words in destination to value.
	/// @param destination Set destination.
	/// @param value Value to set destination half-words to.
	/// @param nrOfHwords Number of half-words to set.
	extern "C" void memset16(void *destination, uint16_t value, uint32_t nrOfHwords) IWRAM_FUNC;

	/// @brief Set words in destination to value.
	/// @param destination Set destination.
	/// @param value Value to set destination dwords to.
	/// @param nrOfWords Number of words to set.
	extern "C" void memset32(void *destination, uint32_t value, uint32_t nrOfWords) IWRAM_FUNC;

} // namespace Memory

#ifndef TARGET_PC

#ifdef __cplusplus
extern "C"
{
#endif

	/// Wrappers for malloc and free to replace the default functions
	/// Otherwise static variables might end up cluttering IWRAM
	void *__wrap_malloc(uint32_t nrOfBytes);
	void __wrap_free(void *adress);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
