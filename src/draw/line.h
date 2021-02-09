#pragma once

#include "math/vec.h"


struct Line2D
{
    Math::fp1616vec2_t p0;
    Math::fp1616vec2_t p1;
} __attribute__((aligned (4), packed));

struct Line3D
{
    Math::fp1616vec3_t p0;
    Math::fp1616vec3_t p1;
} __attribute__((aligned (4), packed));
