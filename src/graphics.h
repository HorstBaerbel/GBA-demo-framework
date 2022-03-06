#pragma once

#include "fptime.h"
#include "palette.h"

#include <cstdint>

namespace Graphics
{

    /// @brief Clear buffer to color. Use with e.g. backBuffer();
    void clear8(uint16_t *buffer, const uint8_t color = 0) IWRAM_FUNC;

    /// @brief Clear buffer to color. Use with e.g. backBuffer();
    void clear16(uint16_t *buffer, const uint16_t color = 0) IWRAM_FUNC;

    /// @brief Clear a block of video memory.
    /// @param buffer Buffer to clear.
    /// @param x Start pixel, MUST BE ALIGNED TO 4!
    /// @param y Start pixel, MUST BE ALIGNED TO 4!
    /// @param blockSize, MUST BE DIVISIBLE 4!
    /// @param color Guess.
    void clearBlock8(uint16_t *buffer, uint32_t x, uint32_t y, uint32_t blockSize, const uint8_t color = 0) IWRAM_FUNC;

    /// @brief Clear a block of video memory.
    /// @param buffer Buffer to clear.
    /// @param x Start pixel, MUST BE ALIGNED TO 4!
    /// @param y Start pixel, MUST BE ALIGNED TO 4!
    /// @param blockSize, MUST BE DIVISIBLE 4!
    /// @param color Guess.
    void clearBlock16(uint16_t *buffer, uint32_t x, uint32_t y, uint32_t blockSize, const uint16_t color = 0) IWRAM_FUNC;

    /// @brief Set a pixel in buffer to color. Very slow.
    void setPixel8(uint16_t *buffer, uint32_t x, uint32_t y, const uint8_t color) IWRAM_FUNC;

    /// @brief Set 1-2 pixel in buffer to color. Slow.
    void setPixel16(uint16_t *buffer, uint32_t x, uint32_t y, const uint16_t color) IWRAM_FUNC;

    /// @brief Set 2-4 pixel in buffer to color. Slow.
    void setPixel32(uint16_t *buffer, uint32_t x, uint32_t y, const uint32_t color) IWRAM_FUNC;

    /// @brief Fill 16-color buffer with random values.
    /// @param noZeroColor If true does not fill the buffer with the nibble value 0.
    /// @param halfYRes If true will double pixels vertically (might be a bit faster).
    void fillRandom4(uint16_t *buffer, bool noZeroColor = false, bool halfYRes = false);

    /// @brief Fill 256-color buffer with random values.
    /// @param noZeroColor If true does not fill the buffer with the byte value 0.
    /// @param halfYRes If true will double / repeat pixels vertically (might be a bit faster).
    void fillRandom8(uint16_t *buffer, bool noZeroColor = false, bool halfYRes = false);

    /// @brief Blit block of memory to buffer.
    /// @param buffer Buffer to blit to.
    /// @param x Horizontal start coordinate.
    /// @param y Vertical start coordinate.
    /// @param data Bitmal data to copy.
    /// @param width Bitmap width.
    /// @param height Bitmap height.
    void blit4(uint16_t *buffer, int32_t x, int32_t y, const uint8_t *data, uint32_t width, uint32_t height);

    /// @brief Blit block of memory to buffer.
    /// @param buffer Buffer to blit to.
    /// @param x Horizontal start coordinate.
    /// @param y Vertical start coordinate.
    /// @param data Bitmal data to copy.
    /// @param width Bitmap width.
    /// @param height Bitmap height.
    void blit8(uint16_t *buffer, int32_t x, int32_t y, const uint8_t *data, uint32_t width, uint32_t height);

    /// @brief Blit block of memory to buffer.
    /// @param buffer Buffer to blit to.
    /// @param x Horizontal start coordinate.
    /// @param y Vertical start coordinate.
    /// @param data Bitmal data to copy.
    /// @param width Bitmap width.
    /// @param height Bitmap height.
    void blit16(uint16_t *buffer, int32_t x, int32_t y, const uint16_t *data, uint32_t width, uint32_t height);

    /// @brief Blit block of memory to buffer making one color transparent.
    /// @param buffer Buffer to blit to.
    /// @param x Horizontal start coordinate.
    /// @param y Vertical start coordinate.
    /// @param data Bitmal data to copy.
    /// @param width Bitmap width.
    /// @param height Bitmap height.
    /// @param transparent The color index to make transparent.
    void blitTransparent8(uint16_t *buffer, int32_t screenX, int32_t screenY, const uint8_t *data, uint32_t dataWidth, uint32_t dataHeight, uint8_t transparent = 0);

    /// @brief Mode info for buffers. Set this with the same value you would use for REG_DISPSTAT.
    void setMode(const uint16_t modeData);

    /// @brief Mode info for buffers. basically a mirror of REG_DISPSTAT.
    uint16_t getMode();

    /// @brief Current front buffer address.
    uint16_t *frontBuffer();

    /// @brief Current back buffer address. Draw here.
    uint16_t *backBuffer();

    /// @brief Swap the front- and backbuffer.
    /// @param waitForVblank Pass true to make the function wait till the next Vblank before swapping.
    void swap(bool waitForVblank = true);

    /// @brief Horizontal resolution in current graphics mode.
    uint16_t width();

    /// @brief Vertical resolution in current graphics mode.
    uint16_t height();

    // How many bytes every scanline has.
    uint32_t bytesPerScanline();

    // How many bytes every pixel has.
    uint32_t bytesPerPixel();

    /// @brief Wait till currently drawn scan line is == lineNr. Might return instantly.
    /// @param lineNr Line number to wait for.
    void waitUntilVcount(uint16_t lineNr);

    /// @brief Wait till system is in vertical blanking.
    /// @param waitForStart Pass true to wait till the next START of Vblank.
    /// @note If you pass waitForStart = false the function might return at the very end of Vblank and screw up your on-screen rendering...
    void waitForVblank(bool waitForStart = true);

    /// @brief Type of vblank handler functions
    using VblankHandlerFunction = void (*)();

    /// @brief Get the Vblank handler used for video.
    VblankHandlerFunction vblankHandler();

    /// @brief Enable or disable vertical blanking interrupt.
    /// If disabled, functions registered with callAtVblank will not work.
    void vblankEnable(bool enable = true);

    /// @brief Register a function to be called when the system enters Vblank.
    void callAtVblank(void (*function)(void *), void *data);

    /// @brief Register a function to be called when the system enters Vblank.
    void callAtVblank(void (*function)());

    /// @brief Unregister a function to be called when the system enters Vblank.
    void removeAtVblank(void (*function)(void *), const void *data = nullptr);

    /// @brief Unregister a function to be called when the system enters Vblank.
    void removeAtVblank(void (*function)());

    /// @brief Unregister all functions to be called when the system enters Vblank.
    void clearAtVblank();

    /// @brief Enable or disable vertical couting interrupt.
    /// If disabled, functions registered with callAtVcount will not work.
    /// @note Enabling vcount interrupts is rather expensive.
    void vcountEnable(bool enable = true);

    /// @brief Register a function to be called when a specific screen display line is drawn.
    /// @note You MUST insert functions in increasing startLine order!
    /// Also endLine must be >= startLine. If endLine > 227 it will be ignored.
    void callAtVcount(void (*function)(void *), void *data, uint16_t startLine, uint16_t endLine = UINT16_MAX);

    /// @brief Register a function to be called when a specific screen display line is drawn.
    /// @note You MUST insert functions in increasing startLine order!
    /// Also endLine must be >= startLine. If endLine > 227 it will be ignored.
    void callAtVcount(void (*function)(), uint16_t startLine, uint16_t endLine = UINT16_MAX);

    /// @brief Unregister a function to be called when a specific screen display line is drawn.
    void removeAtVcount(void (*function)(void *), const void *data = nullptr);

    /// @brief Unregister all functions to be called when a specific screen display line is drawn.
    void clearAtVcount();

} //namespace Video
