#include "draw_gradient.h"
#include "debug/print.h"

/*void drawGradient_mode4(uint16_t *buffer, const Gradient8 &gradient, const Math::fp1616vec2_t &from, const Math::fp1616vec2_t &to)
{
    // get some video stuff
    const uint32_t wordsPerScanline = Video::bytesPerScanline() >> 1;
    const int32_t width = Video::width();
    const int32_t height = Video::height();
    // calculate normalized direction
    Math::fp1616vec2_t dir = to - from;
    Math::fp1616_t dirLength = dir.length();
    Math::fp1616vec2_t unitDir = dir.normalized();
    // calculate upper-left, upper-right and bottom-left corner position vector projection onto direction vector
    Math::fp1616vec2_t ul = Math::fp1616vec2_t(0, 0) - from;
    Math::fp1616vec2_t ur = Math::fp1616vec2_t(width, 0) - from;
    Math::fp1616vec2_t bl = Math::fp1616vec2_t(0, height) - from;
    //printf("from = %d, to = %d\n", from, to);
    //printf("ul = %d, ur = %d, bl = %d\n", ul, ur, bl);
    Math::fp1616_t tul = ul.dot(unitDir);
    Math::fp1616_t tur = ur.dot(unitDir);
    Math::fp1616_t tbl = bl.dot(unitDir);
    //printf("tul = %d, tur = %d, tbl = %d\n", tul, tur, tbl);
    // calculate gradient along direction
    Math::fp1616_t gto = Math::fp1616_t(gradient.to);
    Math::fp1616_t gfrom = Math::fp1616_t(gradient.from);
    Math::fp1616_t gDelta = (gto - gfrom) / dirLength;
    //printf("gradient.to = %d, gradient.from = %d, gDelta = %d\n", gto, gfrom, gDelta);
    // calculate upper-left and upper-right and bottom-left corner values
    const Math::fp1616_t vul = tul * gDelta + gfrom;
    Math::fp1616_t vur = tur * gDelta + gfrom;
    Math::fp1616_t vbl = tbl * gDelta + gfrom;
    //printf("vul = %d, vur = %d, vbl = %d\n", vul, vur, vbl);
    // calculate step values in horizontal and vertical direction
    const Math::fp1616_t steph = (vur - vul) / width;
    const Math::fp1616_t stepv = (vbl - vul) / height;
    //printf("steph = %d, stepv = %d\n", steph, stepv);
    const Math::fp1616_t gMin = gfrom < gto ? gfrom : gto;
    const Math::fp1616_t gMax = gfrom < gto ? gto : gfrom;
    //printf("gMin = %d, gMax = %d\n", gMin, gMax);
    // now interpolate from upper-left to bottom-right
    Math::fp1616_t v = vul;
    for (int32_t y = 0; y < height; ++y)
    {
        Math::fp1616_t oldValue = v;
        for (int32_t x = 0; x < (width >> 1); ++x)
        {
            uint16_t cv0 = (uint16_t)clamp(v, gMin, gMax);
            v += steph;
            uint16_t cv1 = (uint16_t)clamp(v, gMin, gMax);
            v += steph;
            buffer[x] = (cv1 << 8) | cv0;
        }
        v = oldValue + stepv;
        buffer += wordsPerScanline;
    }
}*/

void drawGradient_mode4(uint16_t *buffer, const Gradient8 &gradient, const Math::fp1616vec2_t &from, const Math::fp1616vec2_t &to)
{
    // get some video stuff
    const uint32_t wordsPerScanline = Video::bytesPerScanline() >> 1;
    const int32_t width = Video::width();
    const int32_t height = Video::height();
    // calculate upper-left, upper-right and bottom-left corner position
    Math::fp1616vec2_t ul = Math::fp1616vec2_t(0, 0) - from;
    Math::fp1616vec2_t ur = Math::fp1616vec2_t(width - 1, 0) - from;
    Math::fp1616vec2_t bl = Math::fp1616vec2_t(0, height - 1) - from;
    //printf("from = %d, to = %d", from, to);
    //printf("ul = %d, ur = %d, bl = %d", ul, ur, bl);
    // calculate direction
    Math::fp1616vec2_t dir = to - from;
    Math::fp1616_t dirLen = dir.normalizeAndReturnLength();
    Math::fp1616_t gto = gradient.to;
    Math::fp1616_t gfrom = gradient.from;
    // calculate gradient
    Math::fp1616_t gx = ((gto - gfrom) * dir.x) / dirLen;
    Math::fp1616_t gy = ((gto - gfrom) * dir.y) / dirLen;
    //printf("dir = %d", dir);
    //printf("gx = %d, gy = %d", gx, gy);
    Math::fp1616_t vul = gx * ul.x + gy * ul.y + gfrom;
    Math::fp1616_t vur = gx * ur.x + gy * ur.y + gfrom;
    Math::fp1616_t vbl = gx * bl.x + gy * bl.y + gfrom;
    //printf("vul = %d, vur = %d, vbl = %d", vul, vur, vbl);
    // calculate step values in horizontal and vertical direction
    const Math::fp1616_t steph = (vur - vul) / width;
    const Math::fp1616_t stepv = (vbl - vul) / height;
    //printf("steph = %d, stepv = %d", steph, stepv);
    //const Math::fp1616_t gMin = gfrom < gto ? gfrom : gto;
    //const Math::fp1616_t gMax = gfrom < gto ? gto : gfrom;
    //printf("gMin = %d, gMax = %d", gMin, gMax);
    // now interpolate from upper-left to bottom-right
    for (int32_t y = 0; y < height; ++y)
    {
        Math::fp1616_t v = vul + stepv * y;
        for (uint32_t x = 0; x < wordsPerScanline; ++x)
        {
            uint16_t cv0 = (uint16_t)v; //(uint16_t)clamp(v, gMin, gMax);
            v += steph;
            uint16_t cv1 = (uint16_t)v; //(uint16_t)clamp(v, gMin, gMax);
            v += steph;
            buffer[x] = (cv1 << 8) | cv0;
        }
        buffer += wordsPerScanline;
    }
}