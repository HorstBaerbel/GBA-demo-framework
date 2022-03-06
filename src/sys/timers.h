#pragma once

#include "base.h"

#define REG_TM0CNT *(volatile uint32_t *)(REG_BASE + 0x100)
#define REG_TM0CNT_L *(volatile uint16_t *)(REG_BASE + 0x100)
#define REG_TM0CNT_H *(volatile uint16_t *)(REG_BASE + 0x102)

#define REG_TM1CNT *(volatile uint32_t *)(REG_BASE + 0x104)
#define REG_TM1CNT_L *(volatile uint16_t *)(REG_BASE + 0x104)
#define REG_TM1CNT_H *(volatile uint16_t *)(REG_BASE + 0x106)

#define REG_TM2CNT *(volatile uint32_t *)(REG_BASE + 0x108)
#define REG_TM2CNT_L *(volatile uint16_t *)(REG_BASE + 0x108)
#define REG_TM2CNT_H *(volatile uint16_t *)(REG_BASE + 0x10a)

#define REG_TM3CNT *(volatile uint32_t *)(REG_BASE + 0x10c)
#define REG_TM3CNT_L *(volatile uint16_t *)(REG_BASE + 0x10c)
#define REG_TM3CNT_H *(volatile uint16_t *)(REG_BASE + 0x10e)

#define TIMER_COUNT BIT(2)
#define TIMER_IRQ BIT(6)
#define TIMER_START BIT(7)
