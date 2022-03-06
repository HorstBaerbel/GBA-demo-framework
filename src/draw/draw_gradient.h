#pragma once

#include "gradient.h"
#include "sys/video.h"
#include "math/vec.h"

void draw_gradient(uint16_t *buffer, const Gradient8 &gradient, const Math::fp1616vec2_t &start, const Math::fp1616vec2_t &end);
