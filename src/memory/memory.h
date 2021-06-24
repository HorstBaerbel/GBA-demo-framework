#pragma once

#include <gba_base.h>
#include <cstdint>

/// @brief Use for default 0 initialized static variables
#define STATIC_BSS static EWRAM_BSS
/// @brief Use for initialized static variables
#define STATIC_DATA static EWRAM_DATA

namespace Memory
{

	// PLEASE NOTE: We're on ARM, so word means 32-bits, half-word means 16-bit!

	/// @brief Effectively clear all RAM and initializes the management structures.
	void init();

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

	/// @brief Copy dwords from source to destination.
	/// @param destination Copy destination.
	/// @param source Copy source.
	/// @param nrOfWords Number of dwords to copy.
	extern "C" void memcpy32(void *destination, const void *source, uint32_t nrOfWords) IWRAM_CODE;

	/// @brief Set words in destination to value.
	/// @param destination Set destination.
	/// @param value Value to set destination half-words to.
	/// @param nrOfHwords Number of half-words to set.
	extern "C" void memset16(void *destination, uint16_t value, uint32_t nrOfHwords) IWRAM_CODE;

	/// @brief Set dwords in destination to value.
	/// @param destination Set destination.
	/// @param value Value to set destination dwords to.
	/// @param nrOfWords Number of dwords to set.
	extern "C" void memset32(void *destination, uint32_t value, uint32_t nrOfWords) IWRAM_CODE;

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
