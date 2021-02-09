#pragma once

#include <stdint.h>

char *itoa(uint32_t value, char *result, uint32_t base = 10);
char *itoa(int32_t value, char *result, uint32_t base = 10);
char *itoa(uint64_t value, char *result, uint32_t base = 10);
char *itoa(int64_t value, char *result, uint32_t base = 10);
char *fptoa(int32_t value, char *result, uint32_t BITSF);
