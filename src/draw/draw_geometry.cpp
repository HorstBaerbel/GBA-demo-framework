#include "draw_geometry.h"
#include "memory/dma.h"
#include "graphics.h"

void draw_line(uint16_t *buffer, const Line2D &line, color8 color)
{
    int16_t x0, x1, y0, y1;
    // make sure the line runs top->bottom
    if (line.p0.y > line.p1.y)
    {
        x0 = (int16_t)line.p1.x;
        y0 = (int16_t)line.p1.y;
        x1 = (int16_t)line.p0.x;
        y1 = (int16_t)line.p0.y;
    }
    else
    {
        x0 = (int16_t)line.p0.x;
        y0 = (int16_t)line.p0.y;
        x1 = (int16_t)line.p1.x;
        y1 = (int16_t)line.p1.y;
    }
    // calculate gradients
    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    // calculate direction and swap if necessary
    int16_t dir = 1;
    if (dx < 0)
    {
        dir = -dir;
        dx = -dx;
    }
    // draw first pixel
    Graphics::setPixel8(buffer, x0, y0, color);
    // simple horizontal line?
    if (dy == 0)
    {
        while (dx--)
        {
            Graphics::setPixel8(buffer, x0++, y0, color);
        }
    }
    // simple vertical line?
    else if (dx == 0)
    {
        while (dy--)
        {
            Graphics::setPixel8(buffer, x0, y0++, color);
        }
    }
    // line more vertical than horizontal?
    else if (dy > dx)
    {
        uint16_t errorAcc = 0;
        uint16_t errorAdj = (dx << (sizeof(errorAdj) * 8)) / dy;
        while (dy--)
        {
            uint16_t errorTemp = errorAcc;
            errorAcc += errorAdj;
            // check if error overflowed
            if (errorAcc <= errorTemp)
            {
                x0 += dir;
            }
            y0++;
            Graphics::setPixel8(buffer, x0, y0, color);
        }
    }
    // line more horizontal than vertical
    else
    {
        uint16_t errorAcc = 0;
        uint16_t errorAdj = (dy << (sizeof(errorAdj) * 8)) / dx;
        while (dx--)
        {
            uint16_t errorTemp = errorAcc;
            errorAcc += errorAdj;
            // check if error overflowed
            if (errorAcc <= errorTemp)
            {
                y0++;
            }
            x0 += dir;
            Graphics::setPixel8(buffer, x0, y0, color);
        }
    }
}
