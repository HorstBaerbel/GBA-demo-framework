#include <debug/print.h>
#include <memory/memory.h>

//disable GCC warnings for using char * here...
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace Test
{

    using namespace Memory;

    struct Block
    {
        void *adress = nullptr;
        uint32_t size = 0;
    };

#define NR_OF_BLOCKS 6
    Block TestBlocks[NR_OF_BLOCKS] = {{nullptr, 7}, {nullptr, 128}, {nullptr, 18}, {nullptr, 1024}, {nullptr, 14}, {nullptr, 14}};
    const int BlockOrder0[NR_OF_BLOCKS] = {0, 1, 2, 3, 4, 5};
    const int BlockOrder1[NR_OF_BLOCKS] = {2, 0, 1, 5, 3, 4};
    const int BlockOrder2[NR_OF_BLOCKS] = {5, 4, 3, 2, 1, 0};

    void malloc_and_free(void *(*mallocFunc)(uint32_t), const int (&orderMalloc)[NR_OF_BLOCKS], const int (&orderFree)[NR_OF_BLOCKS])
    {
        Memory::init();
        for (int i = 0; i < NR_OF_BLOCKS; ++i)
        {
            TestBlocks[orderMalloc[i]].adress = mallocFunc(TestBlocks[orderMalloc[i]].size);
        }
        for (int i = 0; i < NR_OF_BLOCKS; ++i)
        {
            free(TestBlocks[orderFree[i]].adress);
        }
    }

    void memory()
    {
        printf("Memory manager tests...\n");
        printf("Testing EWRAM malloc/free...");
        malloc_and_free(malloc_EWRAM, BlockOrder0, BlockOrder0);
        malloc_and_free(malloc_EWRAM, BlockOrder0, BlockOrder1);
        malloc_and_free(malloc_EWRAM, BlockOrder0, BlockOrder2);
        malloc_and_free(malloc_EWRAM, BlockOrder1, BlockOrder0);
        malloc_and_free(malloc_EWRAM, BlockOrder1, BlockOrder1);
        malloc_and_free(malloc_EWRAM, BlockOrder1, BlockOrder2);
        malloc_and_free(malloc_EWRAM, BlockOrder2, BlockOrder0);
        malloc_and_free(malloc_EWRAM, BlockOrder2, BlockOrder1);
        malloc_and_free(malloc_EWRAM, BlockOrder2, BlockOrder2);
        printf("done\n");
        printf("Testing IWRAM malloc/free...");
        malloc_and_free(malloc_IWRAM, BlockOrder0, BlockOrder0);
        malloc_and_free(malloc_IWRAM, BlockOrder0, BlockOrder1);
        malloc_and_free(malloc_IWRAM, BlockOrder0, BlockOrder2);
        malloc_and_free(malloc_IWRAM, BlockOrder1, BlockOrder0);
        malloc_and_free(malloc_IWRAM, BlockOrder1, BlockOrder1);
        malloc_and_free(malloc_IWRAM, BlockOrder1, BlockOrder2);
        malloc_and_free(malloc_IWRAM, BlockOrder2, BlockOrder0);
        malloc_and_free(malloc_IWRAM, BlockOrder2, BlockOrder1);
        malloc_and_free(malloc_IWRAM, BlockOrder2, BlockOrder2);
        printf("done\n");
    }

} // namespace Test