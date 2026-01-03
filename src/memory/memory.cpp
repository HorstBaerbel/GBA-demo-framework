#include "memory.h"

// #define DEBUG_MEMORY
#ifdef DEBUG_MEMORY
#include "print/output.h"
#define GUARD_VALUE 0xDEADBEEF
#endif

extern uint32_t __iheap_start; // Points to the start of free IWRAM
extern uint32_t __sp_usr;	   // Points to the end of the stack in IWRAM, which grows in direction of __iheap_start
extern uint32_t __eheap_start; // Points to the start of free EWRAM
extern uint32_t __eheap_end;   // Points to the end of free EWRAM

namespace Memory
{
	// Simple memory heap manager for IWRAM/EWRAM. Heap memory grows towards the end of the reserved memory
	// Management structures are at the very start of each block.
	// After the management structure follows the allocated memory.
	// Management structures are all kept in the specific RAM they're for.

	constexpr uint32_t StackSize = 1024; // Amount of memory to reserve for stack
	IWRAM_DATA bool m_isInitialized = false;

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
		MemoryBlock *block_iwram = reinterpret_cast<MemoryBlock *>(&__iheap_start);
#ifdef DEBUG_MEMORY
		block_iwram->GUARD = GUARD_VALUE;
#endif
		block_iwram->size = (reinterpret_cast<uint32_t>(&__sp_usr) - StackSize - reinterpret_cast<uint32_t>(&__iheap_start)) - sizeof(MemoryBlock);
		block_iwram->free = true;
		block_iwram->previous = nullptr;
		block_iwram->next = nullptr;
		MemoryBlock *block_ewram = reinterpret_cast<MemoryBlock *>(&__eheap_start);
#ifdef DEBUG_MEMORY
		block_ewram->GUARD = GUARD_VALUE;
#endif
		block_ewram->size = (reinterpret_cast<uint32_t>(&__eheap_end) - reinterpret_cast<uint32_t>(&__eheap_start)) - sizeof(MemoryBlock);
		block_ewram->free = true;
		block_ewram->previous = nullptr;
		block_ewram->next = nullptr;
#ifdef DEBUG_MEMORY
		Debug::printf("Reserving %d bytes of IWRAM starting at 0x%x", uint32_t(block_iwram->size), reinterpret_cast<uint32_t>(block_iwram));
		Debug::printf("Reserving %d bytes of EWRAM starting at 0x%x", uint32_t(block_ewram->size), reinterpret_cast<uint32_t>(block_ewram));
		Debug::printf("sizeof(MemoryBlock) is %d bytes", (uint16_t)sizeof(MemoryBlock));
#endif
	}

#ifdef DEBUG_MEMORY
	void checkMemory()
	{
		MemoryBlock *iwBlock = reinterpret_cast<MemoryBlock *>(&__iheap_start);
		if (iwBlock->GUARD != GUARD_VALUE)
		{
			Debug::printf("Memory corruption in IWRAM block 0x%x!", iwBlock);
		}
		while (iwBlock->next != nullptr)
		{
			iwBlock = iwBlock->next;
			if (iwBlock->GUARD != GUARD_VALUE)
			{
				Debug::printf("Memory corruption in IWRAM block 0x%x!", iwBlock);
			}
		}
		MemoryBlock *ewBlock = reinterpret_cast<MemoryBlock *>(&__eheap_start);
		if (ewBlock->GUARD != GUARD_VALUE)
		{
			Debug::printf("Memory corruption in EWRAM block 0x%x!", ewBlock);
		}
		while (ewBlock->next != nullptr)
		{
			ewBlock = ewBlock->next;
			if (ewBlock->GUARD != GUARD_VALUE)
			{
				Debug::printf("Memory corruption in EWRAM block 0x%x!", ewBlock);
			}
		}
	}
#endif

#ifdef DEBUG_MEMORY
	void *malloc_internal(uint32_t size, uint8_t *memory, const char *name)
	{
		if (!m_isInitialized)
		{
			m_isInitialized = true;
			init();
		}
		checkMemory();
#else
	void *malloc_internal(uint32_t size, uint8_t *memory)
	{
		if (!m_isInitialized)
		{
			m_isInitialized = true;
			init();
		}
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
					Debug::printf("Allocating %d bytes of %s at 0x%x", size, name, reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock));
#endif
					// check if the remaining size would be bigger than management data
					if (currentBlock->size - combinedSizeNeeded > sizeof(MemoryBlock))
					{
						// yes. insert free block behind this one
						MemoryBlock *freeBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<uint8_t *>(currentBlock) + combinedSizeNeeded);
						freeBlock->size = currentBlock->size - combinedSizeNeeded;
#ifdef DEBUG_MEMORY
						Debug::printf("Inserting free block with %d bytes of %s at 0x%x", uint32_t(freeBlock->size), name, reinterpret_cast<uint32_t>(freeBlock) + sizeof(MemoryBlock));
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
			Debug::printf("Out of %s! Failed to allocate %d bytes!", name, size);
#endif
		}
		return nullptr;
	}

	void *malloc_IWRAM(uint32_t size)
	{
#ifdef DEBUG_MEMORY
		return malloc_internal(size, (uint8_t *)&__iheap_start, "IWRAM");
#else
		return malloc_internal(size, (uint8_t *)&__iheap_start);
#endif
	}

	void *malloc_EWRAM(uint32_t size)
	{
#ifdef DEBUG_MEMORY
		return malloc_internal(size, (uint8_t *)&__eheap_start, "EWRAM");
#else
		return malloc_internal(size, (uint8_t *)&__eheap_start);
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
		if ((adress >= &__iheap_start && adress < (&__sp_usr - StackSize + sizeof(void *))) ||
			(adress >= &__eheap_start && adress < (&__eheap_end + sizeof(void *))))
		{
			// memory points to start of memory. skip to management structure from there
			MemoryBlock *currentBlock = reinterpret_cast<MemoryBlock *>(static_cast<uint8_t *>(adress) - sizeof(MemoryBlock));
			// check if used
			if (!currentBlock->free)
			{
#ifdef DEBUG_MEMORY
				Debug::printf("Freeing %d bytes at 0x%x", (uint32_t)currentBlock->size, adress);
#endif
				// mark block as free now
				currentBlock->free = true;
				// are there blocks behind this? are they free?
				while (currentBlock->next != nullptr && currentBlock->next->free)
				{
#ifdef DEBUG_MEMORY
					Debug::printf("Combining free blocks current 0x%x and next 0x%x", reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock), reinterpret_cast<uint32_t>(currentBlock->next) + sizeof(MemoryBlock));
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
					Debug::printf("Combining free blocks previous 0x%x and current 0x%x", reinterpret_cast<uint32_t>(currentBlock->previous) + sizeof(MemoryBlock), reinterpret_cast<uint32_t>(currentBlock) + sizeof(MemoryBlock));
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
			Debug::print("Adress not in IWRAM or EWRAM heap!");
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