#pragma once

#include "base.h"

#include <cstdint>

#define SND1_R_ENABLE (1 << 8)
#define SND1_L_ENABLE (1 << 12)
#define SND2_R_ENABLE (1 << 9)
#define SND2_L_ENABLE (1 << 13)
#define SND3_R_ENABLE (1 << 10)
#define SND3_L_ENABLE (1 << 14)
#define SND4_R_ENABLE (1 << 11)
#define SND4_L_ENABLE (1 << 15)

#define SNDA_VOL_50 (0 << 2)
#define SNDA_VOL_100 (1 << 2)
#define SNDB_VOL_50 (0 << 3)
#define SNDB_VOL_100 (1 << 3)
#define SNDA_R_ENABLE (1 << 8)
#define SNDA_L_ENABLE (1 << 9)
#define SNDA_RESET_FIFO (1 << 11)
#define SNDB_R_ENABLE (1 << 12)
#define SNDB_L_ENABLE (1 << 13)
#define SNDB_RESET_FIFO (1 << 15)

#define REG_SOUNDCNT_L (*((uint16_t volatile *)(REG_BASE + 0x080)))
#define REG_SOUNDCNT_H (*((uint16_t volatile *)(REG_BASE + 0x082)))
#define REG_SOUNDCNT_X (*((uint16_t volatile *)(REG_BASE + 0x084)))

#define REG_SOUND1CNT_L (*((uint16_t volatile *)(REG_BASE + 0x060)))
#define REG_SOUND1CNT_H (*((uint16_t volatile *)(REG_BASE + 0x062)))
#define REG_SOUND1CNT_X (*((uint16_t volatile *)(REG_BASE + 0x064)))

#define REG_SOUND2CNT_L (*((uint16_t volatile *)(REG_BASE + 0x068)))
#define REG_SOUND2CNT_H (*((uint16_t volatile *)(REG_BASE + 0x06C)))

#define REG_SOUND3CNT_L (*((uint16_t volatile *)(REG_BASE + 0x070)))
#define REG_SOUND3CNT_H (*((uint16_t volatile *)(REG_BASE + 0x072)))
#define REG_SOUND3CNT_X (*((uint16_t volatile *)(REG_BASE + 0x074)))

#define REG_SOUND4CNT_L (*((uint16_t volatile *)(REG_BASE + 0x078)))
#define REG_SOUND4CNT_H (*((uint16_t volatile *)(REG_BASE + 0x07C)))

#define REG_SOUNDBIAS (*((uint16_t volatile *)(REG_BASE + 0x088)))

#define REG_FIFO_A (*((uint32_t volatile *)(REG_BASE + 0x0A0)))
#define REG_FIFO_B (*((uint32_t volatile *)(REG_BASE + 0x0A4)))

#define WAVE_RAM ((uint16_t volatile *)(REG_BASE + 0x090))

#define SOUND3_STEP32 (0 << 5)     // Use two banks of 32 steps each
#define SOUND3_STEP64 (1 << 5)     // Use one bank of 64 steps
#define SOUND3_SETBANK(n) (n << 6) // Bank to play 0 or 1 (non set bank is written to)
#define SOUND3_PLAY (1 << 7)       // Output sound
#define SOUND3_STOP (0 << 7)       // Stop sound output
