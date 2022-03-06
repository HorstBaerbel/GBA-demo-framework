#include "input.h"

typedef struct
{
    uint16_t Up;
    uint16_t Down;
    uint16_t Held;
    uint16_t Last;
    uint16_t DownRepeat;
} __attribute__((packed)) KeyInput;

static KeyInput Keys = {0, 0, 0, 0, 0};

static constexpr uint8_t delay = 60;
static constexpr uint8_t repeat = 30;
static uint8_t count = 60;

//---------------------------------------------------------------------------------
void scanKeys(void)
//---------------------------------------------------------------------------------
{
    Keys.Last = Keys.Held;
    Keys.Held = (REG_KEYINPUT & 0x03ff) ^ 0x03ff; // upper 6 bits clear on hw not emulated
    uint16_t pressed = Keys.Held & (Keys.Last ^ 0x03ff);
    Keys.DownRepeat |= pressed;
    Keys.Down |= pressed;
    uint16_t released = ((Keys.Held ^ 0x03ff) & Keys.Last);
    Keys.Up |= released;
    Keys.Down &= ~released;
    Keys.DownRepeat &= ~released;
    Keys.Up &= ~pressed;
    if (Keys.Last != Keys.Held)
    {
        count = delay;
    }
    if (delay != 0)
    {
        count--;
        if (count == 0)
        {
            count = repeat;
            Keys.DownRepeat |= Keys.Held;
        }
    }
}

uint16_t keysDown(void)
{
    auto tmp = Keys.Down;
    Keys.Down = 0;
    return tmp;
}
