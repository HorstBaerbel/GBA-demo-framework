#include "memory.h"

//#define DEBUG_MEMORY
#ifdef DEBUG_MEMORY
#include "debug_print.h"
#define GUARD_VALUE 0xDEADBEEF
#endif

namespace Memory
{

	// Simple memory heap manager for IWRAM/EWRAM. Heap memory grows towards the end of the reserved memory,
	// management structures are at the very start of each block.
	// After the management structure follows the allocated memory.
	// Management structures are all kept in the specific RAM they're for.

	alignas(4) uint8_t memory_iwram[IWRAM_RESERVE] IWRAM_DATA;
	alignas(4) uint8_t memory_ewram[EWRAM_RESERVE] EWRAM_DATA;

	/// @brief Structure for one block of allocated or free memory.
	struct MemoryBlock
	{
#ifdef DEBUG_MEMORY
		uint32_t GUARD;
#endif
		struct
		{
			unsigned int size : 24; // size of allocated memory returned to user
			unsigned int free : 8;	// true if the block is free memory.
		};
		MemoryBlock *previous; // pointer to previous block. nullptr if first block.
		MemoryBlock *next;	   // pointer to next block. nullptr if last block.
	} __attribute__((aligned(4), packed));

	void init()
	{
		// clear all memory if existant. we reserve one big free block at the start of the memory pool for both iwram and ewram.
		MemoryBlock *block_iwram = reinterpret_cast<MemoryBlock *>(memory_iwram);
#ifdef DEBUG_MEMORY
		block_iwram->GUARD = GUARD_VALUE;
#endif
		block_iwram->size = IWRAM_RESERVE - sizeof(MemoryBlock);
		block_iwram->free = true;
		block_iwram->previous = nullptr;
		block_iwram->next = nullptr;
		MemoryBlock *block_ewram = reinterpret_cast<MemoryBlock *>(memory_ewram);
#ifdef DEBUG_MEMORY
		block_ewram->GUARD = GUARD_VALUE;
#endif
		block_ewram->size = EWRAM_RESERVE - sizeof(MemoryBlock);
		block_ewram->free = true;
		block_ewram->previous = nullptr;
		block_ewram->next = nullptr;
#ifdef DEBUG_MEMORY
		printf("Reserving %d bytes of IWRAM starting at 0x%x", uint32_t(block_iwram->size), reinterpret_cast<uint32_t>(block_iwram));
		printf("Reserving %d bytes of EWRAM starting at 0x%x", uint32_t(block_ewram->size), reinterpret_cast<uint32_t>(block_ewram));
		printf("sizeof(MemoryBlock) is %d bytes", (uint16_t)sizeof(MemoryBlock));
#endif
	}

#ifdef DEBUG_MEMORY
	void checkMemory()
	{
		MemoryBlock *iwBlock = reinterpret_cast<MemoryBlock *>(memory_iwram);
		if (iwBlock->GUARD != GUARD_VALUE)
		{
			printf("Memory corruption in IWRAM block 0x%x!", iwBlock);
		}
		while (iwBlock->next != nullptr)
		{
			iwBlock = iwBlock->next;
			if (iwBlock->GUARD != GUARD_VALUE)
			{
				printf("Memory corruption in IWRAM block 0x%x!", iwBlock);
			}
		}
		MemoryBlock *ewBlock = reinterpret_cast<MemoryBlock *>(memory_ewram);
		if (ewBlock->GUARD != GUARD_VALUE)
		{
			printf("Memory corruption in EWRAM block 0x%x!", ewBlock);
		}
		while (ewBlock->next != nullptr)
		{
			ewBlock = ewBlock->next;
			if (ewBlock->GUARD != GUARD_VALUE)
			{
				printf("Memory corruption in EWRAM block 0x%x!", ewBlock);
			}
		}
	}
#endif

#ifdef DEBUG_MEMORY
	void *malloc_internal(uint32_t size, uint8_t *memory, const char *name)
	{
		checkMemory();
#else
	void *malloc_internal(uint32_t size, uint8_t *memory)
	{
#endif
		if (size > 0 && size < 262144)
		{
			// pad the size to a multiple of 4, so all data is properly aligned to 32bit. this gives much faster access!
			size = (size + 3) & 0xFFFFFFFC;
			// calculate how much size we need including the management structures
			const uint32_t combinedSizeNeeded = size + sizeof(MemoryBlock);
			// strategy: find the first free block big enough for size + management structure
			MemoryBlock *currentBlock = reinterpret_cast<MemoryBlock *>(memory);
			do
			{
				// block must be free and big enough
				if (currentBlock->free && currentBlock->size >= combinedSizeNeeded)
				{
#ifdef DEBUG_MEMORY
					printf("Allocating %d bytes of %s at 0x%x", size, name, reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock));
#endif
					// check if the remaining size would be bigger than management data
					if (currentBlock->size - combinedSizeNeeded > sizeof(MemoryBlock))
					{
						// yes. insert free block behind this one
						MemoryBlock *freeBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<uint8_t *>(currentBlock) + combinedSizeNeeded);
						freeBlock->size = currentBlock->size - combinedSizeNeeded;
#ifdef DEBUG_MEMORY
						printf("Inserting free block with %d bytes of %s at 0x%x", uint32_t(freeBlock->size), name, reinterpret_cast<uint32_t>(freeBlock) + sizeof(MemoryBlock));
						freeBlock->GUARD = GUARD_VALUE;
#endif
						freeBlock->free = true;
						freeBlock->previous = currentBlock;
						freeBlock->next = currentBlock->next;
						currentBlock->next = freeBlock;
					}
					// mark current block as used
					currentBlock->size = size;
					currentBlock->free = false;
					// new block starts directly after management structure
					return (reinterpret_cast<uint8_t *>(currentBlock) + sizeof(MemoryBlock));
				}
				// block is not free, or too small. skip to next block
				currentBlock = currentBlock->next;
			} while (currentBlock != nullptr);
			// error. no memory available.
#ifdef DEBUG_MEMORY
			printf("Out of %s! Failed to allocate %d bytes!", name, size);
#endif
		}
		return nullptr;
	}

	void *malloc_IWRAM(uint32_t size)
	{
#ifdef DEBUG_MEMORY
		return malloc_internal(size, memory_iwram, "IWRAM");
#else
		return malloc_internal(size, memory_iwram);
#endif
	}

	void *malloc_EWRAM(uint32_t size)
	{
#ifdef DEBUG_MEMORY
		return malloc_internal(size, memory_ewram, "EWRAM");
#else
		return malloc_internal(size, memory_ewram);
#endif
	}

#ifdef DEBUG_MEMORY
	void free(void *adress)
	{
		checkMemory();
#else
	void free(void *adress)
	{
#endif
		if ((adress >= memory_iwram && adress < (memory_iwram + sizeof(memory_iwram))) ||
			(adress >= memory_ewram && adress < (memory_ewram + sizeof(memory_ewram))))
		{
			// memory points to start of memory. skip to management structure from there
			MemoryBlock *currentBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<uint8_t *>(adress) - sizeof(MemoryBlock));
			// check if used
			if (!currentBlock->free)
			{
#ifdef DEBUG_MEMORY
				printf("Freeing %d bytes at 0x%x", (uint32_t)currentBlock->size, reinterpret_cast<uint32_t>(adress));
#endif
				// mark block as free now
				currentBlock->free = true;
				// are there blocks behind this? are they free?
				while (currentBlock->next != nullptr && currentBlock->next->free)
				{
#ifdef DEBUG_MEMORY
					printf("Combining free blocks current 0x%x and next 0x%x", reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock), reinterpret_cast<uint32_t>(currentBlock->next) + sizeof(MemoryBlock));
#endif
					// combine the two blocks
					currentBlock->size += currentBlock->next->size + sizeof(MemoryBlock);
					currentBlock->next = currentBlock->next->next;
					if (currentBlock->next)
					{
						currentBlock->next->previous = currentBlock;
					}
				}
				// are there blocks before this? are they free?
				while (currentBlock->previous != nullptr && currentBlock->previous->free)
				{
#ifdef DEBUG_MEMORY
					printf("Combining free blocks previous 0x%x and current 0x%x", reinterpret_cast<uint32_t>(currentBlock->previous) + sizeof(MemoryBlock), reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock));
#endif
					// combine the two blocks
					currentBlock->previous->size += currentBlock->size + sizeof(MemoryBlock);
					currentBlock->previous->next = currentBlock->next;
					if (currentBlock->previous->next)
					{
						currentBlock->next->previous = currentBlock->previous;
					}
					currentBlock = currentBlock->previous;
				}
			}
		}
#ifdef DEBUG_MEMORY
		else
		{
			print("Adress not in IWRAM or EWRAM heap!");
		}
#endif
	}

	void free(const void *adress)
	{
		free(const_cast<void *>(adress));
	}

} // namespace Memory

void *__wrap_malloc(uint32_t size)
{
	return Memory::malloc_EWRAM(size);
}

void __wrap_free(void *adress)
{
	Memory::free(adress);
}