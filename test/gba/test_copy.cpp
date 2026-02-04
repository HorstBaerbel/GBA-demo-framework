#include <time.h>
#include <graphics.h>
#include <memory/dma.h>
#include <memory/memory.h>
#include <print/print.h>
#include <sys/syscall.h>

// disable GCC warnings for using char * here...
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
        Math::fp1616_t start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(Graphics::backBuffer(), i, vramCount);
        }
        printf("memset32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        /*start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            CpuFastSet(&i, Graphics::backBuffer(), vramCount | (1 << 24));
        }
        printf("CpuFastSet = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);*/
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(Graphics::backBuffer(), i, vramCount);
        }
        printf("dma_fill32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Filling EWRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(dew, i, bufferCount);
        }
        printf("memset32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        /*start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            CpuFastSet(&i, dew, bufferCount | (1 << 24));
        }
        printf("CpuFastSet = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);*/
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(dew, i, bufferCount);
        }
        printf("dma_fill32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Filling IWRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memset32(diw, i, bufferCount);
        }
        printf("memset32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        /*start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            CpuFastSet(&i, diw, bufferCount | (1 << 24));
        }
        printf("CpuFastSet = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);*/
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_fill32(diw, i, bufferCount);
        }
        printf("dma_fill32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Copying EWRAM to VRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(Graphics::backBuffer(), sew, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(Graphics::backBuffer(), sew, bufferCount);
        }
        printf("memcpy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(Graphics::backBuffer(), reinterpret_cast<const uint32_t *>(sew), bufferCount);
        }
        printf("dma_copy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Copying IWRAM to VRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(Graphics::backBuffer(), siw, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(Graphics::backBuffer(), siw, bufferCount);
        }
        printf("memcpy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(Graphics::backBuffer(), reinterpret_cast<const uint32_t *>(siw), bufferCount);
        }
        printf("dma_copy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Copying EWRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy16(dew, sew, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            Memory::memcpy32(dew, sew, bufferCount);
        }
        printf("memcpy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 1024; ++i)
        {
            DMA::dma_copy32(dew, reinterpret_cast<const uint32_t *>(sew), bufferCount);
        }
        printf("dma_copy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        //--------------------------------------------------------------------------
        printf("Copying IWRAM...\n");
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 2048; ++i)
        {
            Memory::memcpy16(diw, siw, bufferCount * 2);
        }
        printf("memcpy16 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 2048; ++i)
        {
            Memory::memcpy32(diw, siw, bufferCount);
        }
        printf("memcpy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        start = Math::fp1616_t::fromRaw(Time::now());
        for (uint32_t i = 0; i < 2048; ++i)
        {
            DMA::dma_copy32(diw, reinterpret_cast<const uint32_t *>(siw), bufferCount);
        }
        printf("dma_copy32 = %d\n", Math::fp1616_t::fromRaw(Time::now()) - start);
        Time::stop();
        // free all memory again
        Memory::free(siw);
        Memory::free(diw);
        Memory::free(sew);
        Memory::free(dew);
    }

} // namespace Test