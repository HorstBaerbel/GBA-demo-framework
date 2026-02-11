#include "graphics.h"

#include "math/random.h"
#include "memory/dma.h"
#include "memory/memory.h"
#include "sys/interrupts.h"
#include "sys/video.h"

namespace Graphics
{

    struct FunctionEntry
    {
        union
        {
            void (*p0)() = nullptr;
            void (*p1)(void *);
        };
        void *data = nullptr;
        Math::fp1616_t remaining;
        uint16_t startLine = 0;
        uint16_t endLine = 0;
    } __attribute__((aligned(4), packed));

    constexpr uint32_t MaxVideoFunctions = 8;
    FunctionEntry m_vblankFunctions[MaxVideoFunctions];
    FunctionEntry m_vcountFunctions[MaxVideoFunctions];
    uint32_t m_nrOfVblankFunctions = 0;
    uint32_t m_nrOfVcountFunctions = 0;
    uint32_t m_currentVcountFunction = 0;

    uint16_t *m_frontBuffer = nullptr; //!<Buffer being displayed.
    uint16_t *m_backBuffer = nullptr;  //!<Buffer drawn into. Flipped when calling \sa swap().
    uint32_t m_width = 0;              //!<Buffer width.
    uint32_t m_height = 0;             //!<Buffer height.
    uint32_t m_nrOfBytes = 0;          //!<Number of bytes one (of the) framebuffer(s) has.
    uint32_t m_bytesPerPixel = 0;      //!<Number of bytes per pixel in framebuffer.
    uint32_t m_bytesPerScanline = 0;   //!<Bytes each scanline in framebuffer has.

    //---helper functions------------------------------------------------------------------

    void addFunction(FunctionEntry *functions, uint32_t &nrOfFunctions, void (*function)(void *), void *data, uint16_t startLine = 0, uint16_t endLine = 0)
    {
        if (nrOfFunctions < MaxVideoFunctions)
        {
            functions[nrOfFunctions].p1 = function;
            functions[nrOfFunctions].data = data;
            functions[nrOfFunctions].remaining = 0;
            functions[nrOfFunctions].startLine = startLine > 227 ? 227 : startLine;
            functions[nrOfFunctions].endLine = endLine > 227 ? 227 : endLine;
            nrOfFunctions++;
        }
    }

    void addFunction(FunctionEntry *functions, uint32_t &nrOfFunctions, void (*function)(), uint16_t startLine = 0, uint16_t endLine = 0)
    {
        if (nrOfFunctions < MaxVideoFunctions)
        {
            functions[nrOfFunctions].p0 = function;
            functions[nrOfFunctions].data = nullptr;
            functions[nrOfFunctions].remaining = 0;
            functions[nrOfFunctions].startLine = startLine > 227 ? 227 : startLine;
            functions[nrOfFunctions].endLine = endLine > 227 ? 227 : endLine;
            nrOfFunctions++;
        }
    }

    void removeFunction(FunctionEntry *functions, uint32_t &nrOfFunctions, void (*function)(), const void *data)
    {
        for (int32_t i = 0; i < static_cast<int32_t>(nrOfFunctions); i++)
        {
            if ((function != nullptr && function == functions[i].p0) || (data != nullptr && data == functions[i].data))
            {
                // move all entries one to the front
                for (; i < static_cast<int32_t>(nrOfFunctions) - 1; i++)
                {
                    functions[i] = functions[i + 1];
                }
                nrOfFunctions--;
                break;
            }
        }
    }

    void removeFunction(FunctionEntry *functions, uint32_t &nrOfFunctions, void (*function)(void *), const void *data)
    {
        for (int32_t i = 0; i < static_cast<int32_t>(nrOfFunctions); i++)
        {
            if ((function != nullptr && function == functions[i].p1) || (data != nullptr && data == functions[i].data))
            {
                // move all entries one to the front
                for (; i < static_cast<int32_t>(nrOfFunctions) - 1; i++)
                {
                    functions[i] = functions[i + 1];
                }
                nrOfFunctions--;
                break;
            }
        }
    }

    //---vblank functions------------------------------------------------------------------

    void vblank()
    {
        for (uint32_t i = 0; i < m_nrOfVblankFunctions; i++)
        {
            auto &func = m_vblankFunctions[i];
            if (func.p0 != nullptr)
            {
                if (func.data != nullptr)
                {
                    func.p1(func.data);
                }
                else
                {
                    func.p0();
                }
            }
        }
    }

    VblankHandlerFunction vblankHandler()
    {
        return vblank;
    }

    void vblankEnable(bool enable)
    {
        if (enable)
        {
            Irq::setHandler(Irq::Mask::VBlank, vblank);
            Irq::enable(Irq::Mask::VBlank);
        }
        else
        {
            Irq::disable(Irq::Mask::VBlank);
        }
    }

    void waitForVblank(bool waitForStart)
    {
        if (waitForStart)
        {
            while ((REG_DISPSTAT & LCDC_VBL_FLAG))
            {
            }
        }
        while ((REG_DISPSTAT & LCDC_VBL_FLAG) == 0)
        {
        }
    }

    void waitUntilVcount(uint16_t lineNr)
    {
        while (REG_VCOUNT != lineNr)
        {
        }
    }

    void callAtVblank(void (*function)(void *), void *data)
    {
        addFunction(m_vblankFunctions, m_nrOfVblankFunctions, function, data);
    }

    void callAtVblank(void (*function)())
    {
        addFunction(m_vblankFunctions, m_nrOfVblankFunctions, function);
    }

    void removeAtVblank(void (*function)(void *), const void *data)
    {
        removeFunction(m_vblankFunctions, m_nrOfVblankFunctions, function, data);
    }

    void removeAtVblank(void (*function)())
    {
        removeFunction(m_vblankFunctions, m_nrOfVblankFunctions, function, nullptr);
    }

    void clearAtVblank()
    {
        m_nrOfVblankFunctions = 0;
    }

    //---vcount functions------------------------------------------------------------------

    void vcount()
    {
        if (m_nrOfVcountFunctions > 0)
        {
            const uint16_t line = REG_VCOUNT;
            const auto &func = m_vcountFunctions[m_currentVcountFunction];
            // check if the current function should be active
            if (line >= func.startLine)
            {
                // yes. do something?
                if (line <= func.endLine && func.p0 != nullptr)
                {
                    if (func.data != nullptr)
                    {
                        func.p1(func.data);
                    }
                    else
                    {
                        func.p0();
                    }
                }
                // if it should be active on the next scanline, queue it
                if (line < func.endLine)
                {
                    // queue this function on the next line
                    REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | ((line + 1) << 8);
                }
                else
                {
                    // skip to the next function or the first function
                    m_currentVcountFunction = m_currentVcountFunction >= m_nrOfVcountFunctions ? 0 : m_currentVcountFunction + 1;
                    REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | (m_vcountFunctions[m_currentVcountFunction].startLine << 8);
                }
            }
        }
    }

    void vcountEnable(bool enable)
    {
        if (enable)
        {
            // set line for interrupt for first item
            if (m_nrOfVcountFunctions > 0)
            {
                m_currentVcountFunction = 0;
                REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | (m_vcountFunctions[0].startLine << 8);
            }
            Irq::setHandler(Irq::Mask::VCount, vcount);
            Irq::enable(Irq::Mask::VCount);
        }
        else
        {
            Irq::disable(Irq::Mask::VCount);
            // clear line for interrupt for first item
            REG_DISPSTAT = REG_DISPSTAT & 0x00FF;
            m_currentVcountFunction = 0;
        }
    }

    void callAtVcount(void (*function)(void *), void *data, uint16_t startLine, uint16_t endLine)
    {
        endLine = endLine < startLine || endLine > 227 ? startLine : endLine;
        addFunction(m_vcountFunctions, m_nrOfVcountFunctions, function, data, startLine, endLine);
    }

    void callAtVcount(void (*function)(), uint16_t startLine, uint16_t endLine)
    {
        endLine = endLine < startLine || endLine > 227 ? startLine : endLine;
        addFunction(m_vcountFunctions, m_nrOfVcountFunctions, function, startLine, endLine);
    }

    void removeAtVcount(void (*function)(void *), const void *data)
    {
        removeFunction(m_vcountFunctions, m_nrOfVcountFunctions, function, data);
    }

    void removeAtVcount(void (*function)())
    {
        removeFunction(m_vcountFunctions, m_nrOfVcountFunctions, function, nullptr);
    }

    void clearAtVcount()
    {
        m_nrOfVcountFunctions = 0;
    }

    //---misc stuff----------------------------------------------------------------

    void setMode(const uint16_t modeData)
    {
        REG_DISPCNT = modeData;
        // check which mode we're in and set pointers accordingly
        const uint16_t mode = modeData & 0b111;
        if (MODE_0 <= mode && MODE_2 >= mode)
        {
            // Note: These modes are tile-based, so basically there's no front/back buffer...
            m_frontBuffer = (uint16_t *)MODE5_FB;
            m_backBuffer = (uint16_t *)MODE5_FB;
            m_bytesPerPixel = 1;
            m_bytesPerScanline = 240;
            m_nrOfBytes = 240 * 160;
            m_width = 240;
            m_height = 160;
        }
        else if (MODE_3 == mode)
        {
            m_frontBuffer = (uint16_t *)MODE3_FB;
            m_backBuffer = (uint16_t *)MODE3_FB;
            m_bytesPerPixel = 2;
            m_bytesPerScanline = 240 * 2;
            m_nrOfBytes = 240 * 160 * 2;
            m_width = 240;
            m_height = 160;
        }
        else if (MODE_4 == mode)
        {
            // check the display control register. we might have switched buffers already
            m_frontBuffer = REG_DISPCNT & BACKBUFFER ? (uint16_t *)MODE5_BB : (uint16_t *)MODE5_FB;
            m_backBuffer = REG_DISPCNT & BACKBUFFER ? (uint16_t *)MODE5_FB : (uint16_t *)MODE5_BB;
            m_bytesPerPixel = 1;
            m_bytesPerScanline = 240;
            m_nrOfBytes = 240 * 160;
            m_width = 240;
            m_height = 160;
        }
        else if (MODE_5 == mode)
        {
            // check the display control register. we might have switched buffers already
            m_frontBuffer = REG_DISPCNT & BACKBUFFER ? (uint16_t *)MODE5_BB : (uint16_t *)MODE5_FB;
            m_backBuffer = REG_DISPCNT & BACKBUFFER ? (uint16_t *)MODE5_FB : (uint16_t *)MODE5_BB;
            m_bytesPerPixel = 2;
            m_bytesPerScanline = 160 * 2;
            m_nrOfBytes = 160 * 128 * 2;
            m_width = 160;
            m_height = 128;
        }
        else
        {
            m_frontBuffer = nullptr;
            m_backBuffer = nullptr;
            m_bytesPerPixel = 0;
            m_bytesPerScanline = 0;
            m_nrOfBytes = 0;
            m_width = 0;
            m_height = 0;
        }
    }

    uint16_t getMode()
    {
        return REG_DISPCNT;
    }

    uint16_t *frontBuffer()
    {
        return m_frontBuffer;
    }

    uint16_t *backBuffer()
    {
        return m_backBuffer;
    }

    uint16_t width()
    {
        return m_width;
    }

    uint16_t height()
    {
        return m_height;
    }

    uint32_t bytesPerScanline()
    {
        return m_bytesPerScanline;
    }

    uint32_t bytesPerPixel()
    {
        return m_bytesPerPixel;
    }

    void swap(bool waitForVblank)
    {
        // here we simply wait for Vblank. No need to wait for the start, we're just flipping buffers...
        if (waitForVblank)
        {
            while ((REG_DISPSTAT & LCDC_VBL_FLAG) == 0)
            {
            }
        }
        // toggle backbuffer bit
        REG_DISPCNT ^= BACKBUFFER;
        // swap pointers
        uint16_t *temp = m_backBuffer;
        m_backBuffer = m_frontBuffer;
        m_frontBuffer = temp;
    }

    void clear8(uint16_t *buffer, const uint8_t color)
    {
        const uint32_t value = ((uint32_t)color << 24) | ((uint32_t)color << 16) | ((uint32_t)color << 8) | ((uint32_t)color);
        Memory::memset32(buffer, value, m_nrOfBytes / 4);
    }

    void clear16(uint16_t *buffer, const uint16_t color)
    {
        const uint32_t value = ((uint32_t)color << 16) | ((uint32_t)color);
        Memory::memset32(buffer, value, m_nrOfBytes / 4);
    }

    void clearBlock8(uint16_t *buffer, uint32_t x, uint32_t y, uint32_t blockSize, const uint8_t color)
    {
        const uint32_t value = color << 24 | color << 16 | color << 8 | color;
        uint32_t *u32buffer = (uint32_t *)(((uint8_t *)buffer) + y * m_bytesPerScanline + x * m_bytesPerPixel);
        if (blockSize == 4)
        {
            *u32buffer = value;
            u32buffer += m_bytesPerScanline >> 2;
            *u32buffer = value;
            u32buffer += m_bytesPerScanline >> 2;
            *u32buffer = value;
            u32buffer += m_bytesPerScanline >> 2;
            *u32buffer = value;
        }
        else
        {
            for (uint32_t yb = 0; yb < blockSize; ++yb)
            {
                for (uint32_t xb = 0; xb < (blockSize >> 2); ++xb)
                {
                    u32buffer[xb] = value;
                }
                // move to next scanline
                u32buffer += m_bytesPerScanline >> 2;
            }
        }
    }

    void clearBlock16(uint16_t *buffer, uint32_t x, uint32_t y, uint32_t blockSize, const uint16_t color)
    {
        const uint32_t value = color << 16 | color;
        uint32_t *u32buffer = (uint32_t *)(((uint8_t *)buffer) + y * m_bytesPerScanline + x * m_bytesPerPixel);
        for (uint32_t yb = 0; yb < blockSize; ++yb)
        {
            for (uint32_t xb = 0; xb < (blockSize >> 1); ++xb)
            {
                u32buffer[xb] = value;
            }
            // move to next scanline
            u32buffer += m_bytesPerScanline >> 2;
        }
    }

    void setPixel8(uint16_t *buffer, uint32_t x, uint32_t y, const uint8_t color)
    {
        // calculate pixel adress. we do this in uint16_t format, because the GBA can only write to vram in word chunks... whyever.
        uint16_t *pixel = &buffer[(y * m_width + x) >> 1];
        // check which byte-sized pixel we're writing to and do a read-modify-write operation
        if (x & 1)
        {
            *pixel = (*pixel & 0xFF) | (color << 8); // odd pixel
        }
        else
        {
            *pixel = (*pixel & ~0xFF) | color; // even pixel
        }
    }

    void setPixel16(uint16_t *buffer, uint32_t x, uint32_t y, const uint16_t color)
    {
        buffer[y * m_width + x] = color;
    }

    void setPixel32(uint16_t *buffer, uint32_t x, uint32_t y, const uint32_t color)
    {
        uint32_t *u32buffer = reinterpret_cast<uint32_t *>(buffer + y * (m_width >> 1) + (x >> 1));
        *u32buffer = color;
    }

    void random(uint16_t *buffer, bool halfYRes, uint32_t orMask)
    {
        uint32_t *buffer32 = reinterpret_cast<uint32_t *>(buffer);
        if (halfYRes)
        {
            const uint32_t dwordsPerLine = m_bytesPerScanline >> 2;
            const uint32_t skipCount = m_bytesPerScanline >> 1;
            for (uint32_t y = 0; y < m_height; y += 2)
            {
                for (uint32_t i = 0; i < dwordsPerLine; i++)
                {
                    uint32_t value = ::random<uint32_t>() | orMask;
                    buffer32[i] = value;
                    buffer32[i + dwordsPerLine] = value;
                }
                buffer32 += skipCount;
            }
        }
        else
        {
            const uint32_t count = m_nrOfBytes >> 2;
            for (uint32_t i = 0; i < count; i++)
            {
                buffer32[i] = ::random<uint32_t>() | orMask;
            }
        }
    }

    void fillRandom4(uint16_t *buffer, bool noZeroColor, bool halfYRes)
    {
        const uint32_t orMask = noZeroColor ? 0x11111111 : 0;
        random(buffer, halfYRes, orMask);
    }

    void fillRandom8(uint16_t *buffer, bool noZeroColor, bool halfYRes)
    {
        const uint32_t orMask = noZeroColor ? 0x01010101 : 0;
        random(buffer, halfYRes, orMask);
    }

    void clampOntoScreen(int32_t &x, int32_t &y, uint32_t &w, uint32_t &h)
    {
        // if the start position is outside of the screen set width / height to 0
        if (x >= static_cast<int32_t>(m_width) || y >= static_cast<int32_t>(m_height))
        {
            w = 0;
            h = 0;
            return;
        }
        // if not check start position in screen
        if (x < 0)
        {
            // starts left of screen, reduce width
            w += x;
            x = 0;
        }
        if (x + w > m_width)
        {
            // extends right of screen, reduce width
            w = m_width - x;
        }
        if (y < 0)
        {
            // starts above of screen, reduce height
            h += y;
            y = 0;
        }
        if (y + h > m_height)
        {
            // extends below of screen, reduce height
            h = m_height - y;
        }
    }

    void blit4(uint16_t *buffer, int32_t screenX, int32_t screenY, const uint8_t *data, uint32_t dataWidth, uint32_t dataHeight)
    {
        // adjust bitmap and screen positions and dimensions
        int32_t bitmapX = screenX < 0 ? -screenX : 0;
        int32_t bitmapY = screenY < 0 ? -screenY : 0;
        uint32_t blitWidth = dataWidth;
        uint32_t blitHeight = dataHeight;
        clampOntoScreen(screenX, screenY, blitWidth, blitHeight);
        if (blitWidth > 0 && blitHeight > 0)
        {
            // calculate blit pixel starts
            const uint32_t bitmapStartIndex = (bitmapY * dataWidth) + bitmapX;
            const uint32_t bufferStartIndex = (screenY * m_width) + screenX;
            uint16_t *dest16 = reinterpret_cast<uint16_t *>(buffer) + bufferStartIndex;
            const uint8_t *src8 = data + bitmapStartIndex;
            // copy scanlines
            for (uint32_t y = 0; y < blitHeight; ++y)
            {
                for (uint32_t x = 0; x < (blitWidth >> 1); ++x)
                {
                    uint16_t c = src8[x];
                    dest16[x] = ((c & 0xF0) << 4) | (c & 0x0F);
                }
                dest16 += m_width >> 1;
                src8 += blitWidth >> 1;
            }
        }
    }

    void blit8(uint16_t *buffer, int32_t screenX, int32_t screenY, const uint8_t *data, uint32_t dataWidth, uint32_t dataHeight)
    {
        // adjust bitmap and screen positions and dimensions
        int32_t bitmapX = screenX < 0 ? -screenX : 0;
        int32_t bitmapY = screenY < 0 ? -screenY : 0;
        uint32_t blitWidth = dataWidth;
        uint32_t blitHeight = dataHeight;
        clampOntoScreen(screenX, screenY, blitWidth, blitHeight);
        if (blitWidth > 0 && blitHeight > 0)
        {
            // calculate blit pixel starts
            const uint32_t bitmapStartIndex = (bitmapY * dataWidth) + bitmapX;
            const uint32_t bufferStartIndex = (screenY * m_width) + screenX;
            uint8_t *dest8 = reinterpret_cast<uint8_t *>(buffer) + bufferStartIndex;
            const uint8_t *src8 = data + bitmapStartIndex;
            // copy scanlines
            for (uint32_t y = 0; y < blitHeight; ++y)
            {
                // TODO: rewrite
                dest8 += m_width;
                src8 += dataWidth;
            }
        }
    }

    void blit16(uint16_t *buffer, int32_t screenX, int32_t screenY, const uint16_t *data, uint32_t dataWidth, uint32_t dataHeight)
    {
        // adjust bitmap and screen positions and dimensions
        int32_t bitmapX = screenX < 0 ? -screenX : 0;
        int32_t bitmapY = screenY < 0 ? -screenY : 0;
        uint32_t blitWidth = dataWidth;
        uint32_t blitHeight = dataHeight;
        clampOntoScreen(screenX, screenY, blitWidth, blitHeight);
        if (blitWidth > 0 && blitHeight > 0)
        {
            // calculate blit pixel starts
            const uint32_t bitmapStartIndex = (bitmapY * dataWidth) + bitmapX;
            const uint32_t bufferStartIndex = (screenY * m_width) + screenX;
            uint16_t *dest16 = buffer + bufferStartIndex;
            const uint16_t *src16 = data + bitmapStartIndex;
            // copy scanlines
            for (uint32_t y = 0; y < blitHeight; ++y)
            {
                Memory::memcpy16(dest16, src16, blitWidth);
                dest16 += m_width;
                src16 += dataWidth;
            }
        }
    }

    void blitTransparent8(uint16_t *buffer, int32_t screenX, int32_t screenY, const uint8_t *data, uint32_t dataWidth, uint32_t dataHeight, uint8_t transparent)
    {
        // adjust bitmap and screen positions and dimensions
        uint32_t bitmapX = screenX < 0 ? -screenX : 0;
        uint32_t bitmapY = screenY < 0 ? -screenY : 0;
        uint32_t blitWidth = dataWidth;
        uint32_t blitHeight = dataHeight;
        clampOntoScreen(screenX, screenY, blitWidth, blitHeight);
        if (blitWidth > 0 && blitHeight > 0)
        {
            const uint16_t transparentWord = ((uint16_t)transparent << 8) | (uint16_t)transparent;
            // calculate blit pixel starts
            uint16_t *dest16 = buffer + ((((uint32_t)screenY * m_width) + (uint32_t)screenX) >> 1);
            const uint16_t *src16 = reinterpret_cast<const uint16_t *>(data) + ((((uint32_t)bitmapY * dataWidth) + (uint32_t)bitmapX) >> 1);
            // copy scanlines
            for (uint32_t y = 0; y < blitHeight; ++y)
            {
                // we need to pre-copy if destination index is odd
                uint32_t nrOfBytes = blitWidth;
                if (((uint32_t)screenX) & 1)
                {
                    // we need to pre-copy 1 byte. replace first byte in destination
                    uint16_t srcPixels = (*src16) << 8;
                    uint16_t dstPixels = *dest16;
                    uint16_t mask = srcPixels ^ transparentWord;
                    uint16_t b = (mask & 0xFF00) ? (srcPixels & 0xFF00) : (dstPixels & 0xFF00);
                    *dest16 = (dstPixels & 0x00FF) | b;
                    dest16++;
                    src16++;
                    nrOfBytes--;
                }
                while (nrOfBytes >= 2)
                {
                    uint16_t srcPixels = *src16;
                    uint16_t dstPixels = *dest16;
                    uint16_t mask = srcPixels ^ transparentWord;
                    uint16_t a = (mask & 0xFF00) ? (srcPixels & 0xFF00) : (dstPixels & 0xFF00);
                    uint16_t b = (mask & 0x00FF) ? (srcPixels & 0x00FF) : (dstPixels & 0x00FF);
                    *dest16 = a | b;
                    dest16++;
                    src16++;
                    nrOfBytes -= 2;
                }
                if (nrOfBytes > 0)
                {
                    // we need to post-copy 1 byte. replace first byte in destination
                    uint16_t srcPixels = (*src16);
                    uint16_t dstPixels = *dest16;
                    uint16_t mask = srcPixels ^ transparentWord;
                    uint16_t b = (mask & 0xFF00) ? (srcPixels & 0xFF00) : (dstPixels & 0xFF00);
                    *dest16 = (dstPixels & 0x00FF) | b;
                }
                dest16 = dest16 + ((m_width - blitWidth + 1) >> 1);
                src16 = src16 + ((dataWidth - blitWidth + 1) >> 1);
            }
        }
    }

} //namespace Video
