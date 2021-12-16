#include <fptime.h>
#include <memory/dma.h>
#include <memory/memory.h>
#include <print/print.h>
#include <sys/syscall.h>
#include <video.h>

//disable GCC warnings for using char * here...
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace Test
{

    void copy()
    {
        printf("Copy function tests...\n");
        Memory::init();
        constexpr uint32_t vramCount = 240 * 160 / 4;
        constexpr uint32_t bufferCount = 1024;
        constexpr uint32_t bufferSize = bufferCount * sizeof(uint32_t);
        uint8_t *sew = static_cast<uint8_t *>(Memory::malloc_EWRAM(bufferSize));
        uint8_t *dew = static_cast<uint8_t *>(Memory::malloc_EWRAM(bufferSize));
        uint8_t *siw = static_cast<uint8_t *>(Memory::malloc_IWRAM(bufferSize));
        uint8_t *diw = static_cast<uint8_t *>(Memory::malloc_IWRAM(bufferSize));
        Time::start();
        //--------------------------------------------------------------------------
        printf("Filling VRAM...\n");
        Math::fp1616_t start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(Video::backBuffer(), i, vramCount);
        }
        printf("memset32 = %d\n", Time::now() - start);
        /*start = Time::now();
		for (uint32_t i = 0; i < 1024; ++i)
		{
			CpuFastSet(&i, Video::backBuffer(), vramCount | (1 << 24));
		}
		printf("CpuFastSet = %d\n", Time::now() - start);*/
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(Video::backBuffer(), i, vramCount);
        }
        printf("dma_fill32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Filling EWRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(dew, i, bufferCount);
        }
        printf("memset32 = %d\n", Time::now() - start);
        /*start = Time::now();
		for (uint32_t i = 0; i < 1024; ++i)
		{
			CpuFastSet(&i, dew, bufferCount | (1 << 24));
		}
		printf("CpuFastSet = %d\n", Time::now() - start);*/
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(dew, i, bufferCount);
        }
        printf("dma_fill32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Filling IWRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(diw, i, bufferCount);
        }
        printf("memset32 = %d\n", Time::now() - start);
        /*start = Time::now();
		for (uint32_t i = 0; i < 1024; ++i)
		{
			CpuFastSet(&i, diw, bufferCount | (1 << 24));
		}
		printf("CpuFastSet = %d\n", Time::now() - start);*/
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(diw, i, bufferCount);
        }
        printf("dma_fill32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Copying EWRAM to VRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(Video::backBuffer(), sew, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(Video::backBuffer(), sew, bufferCount);
        }
        printf("memcpy32 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(Video::backBuffer(), reinterpret_cast<const uint32_t *>(sew), bufferCount);
        }
        printf("dma_copy32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Copying IWRAM to VRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(Video::backBuffer(), siw, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(Video::backBuffer(), siw, bufferCount);
        }
        printf("memcpy32 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(Video::backBuffer(), reinterpret_cast<const uint32_t *>(siw), bufferCount);
        }
        printf("dma_copy32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Copying EWRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(dew, sew, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(dew, sew, bufferCount);
        }
        printf("memcpy32 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(dew, reinterpret_cast<const uint32_t *>(sew), bufferCount);
        }
        printf("dma_copy32 = %d\n", Time::now() - start);
        //--------------------------------------------------------------------------
        printf("Copying IWRAM...\n");
        start = Time::now();
        for (uint32_t i = 0; i < 2048; ++i)
        {
            Memory::memcpy16(diw, siw, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 2048; ++i)
        {
            Memory::memcpy32(diw, siw, bufferCount);
        }
        printf("memcpy32 = %d\n", Time::now() - start);
        start = Time::now();
        for (uint32_t i = 0; i < 2048; ++i)
        {
            DMA::dma_copy32(diw, reinterpret_cast<const uint32_t *>(siw), bufferCount);
        }
        printf("dma_copy32 = %d\n", Time::now() - start);
        Time::stop();
        // free all memory again
        Memory::free(siw);
        Memory::free(diw);
        Memory::free(sew);
        Memory::free(dew);
    }

} // namespace Test