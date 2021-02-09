#include "mode7.h"
#include "blend.h"
#include "affine.h"

//#define INFINITE_HORIZON

namespace Mode7
{

void init(Data &data, Camera::C8DOF * camera, Effect_Affine::AffineData *affineData, Sprite3D *sprites, uint16_t bgcntBackdrop, uint16_t bgcntFloor)
{
    data.camera = camera;
    data.affineData = affineData;
    data.sprites = sprites;
    data.bgcntBackdrop = bgcntBackdrop;
    data.bgcntFloor = bgcntFloor;
    data.horizon = 80;
    REG_BG2CNT = bgcntFloor;
    REG_BG2X = 0;
    REG_BG2Y = 0;
    REG_BG2PA = 0;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0;
}

void prepareHorizon(Data &data)
{
    Math::fp1616_t horizon;
    const Camera *camera = data.camera;
    // calcuate horizon scan line
    if (camera->v.y != 0)
    {
#ifndef INFINITE_HORIZON
        horizon = VIEWPORT_FLOOR_FAR * camera->w.y - camera->position.y;
        horizon = VIEWPORT_TOP - ((horizon * FOCAL_LENGTH) / (VIEWPORT_FLOOR_FAR * camera->v.y));
#else
        horizon = VIEWPORT_TOP - ((camera->w.y * FOCAL_LENGTH) / camera->v.y);
#endif
    }
    else
    {
        // looking straight down (w.y > 0) means horizon at -inf scanline
        horizon = camera->w.y > 0 ? INT_MIN : INT_MAX;
    }
    data.horizon = int16_t(round(horizon));
}

void updateHorizon(const Data &data)
{
    REG_BG2HOFS = int16_t((data.camera->phi * Math::fp1616_t::ONE_OVER_PI_2) >> 10) + VIEWPORT_LEFT;
    REG_BG2VOFS = -horizonLine(data) - 1;
}

void hblankFloor()
{
    const uint16_t vcount = REG_VCOUNT;
    const int16_t horizon = data.horizon;
    // check if out of range
    if (vcount >= SCREEN_HEIGHT || vcount < horizon)
    {
        return;
    }
    // check if first line of floor
    if (vcount == horizon)
    {
        // set display mode to 1 for mode 7
        REG_DISPCNT = (REG_DISPCNT & 0xFFF8) | MODE_1;
        REG_BG2CNT = data.bgcntFloor;
    }
    const Effect_Affine::AffineData &affine = data.affineData[vcount + 1];
    REG_BG2X = affine.refx.raw() >> 8;
    REG_BG2Y = affine.refy.raw() >> 8;
    REG_BG2PA = affine.dx;
    REG_BG2PB = affine.dmx;
    REG_BG2PC = affine.dy;
    REG_BG2PD = affine.dmy;
    // A distance fogging with high marks for hack-value
    // Remember that we used dmx to store the scale in, so the blend is basically lambda/64 = distance * 2
    uint16_t ey = (affine.dmx * 6) >> 12;
    ey = ey > 16 ? 16 : ey;
    REG_BLDCNT = static_cast<uint16_t>(Effect_Blend::Target::TARGET2_BG2) | static_cast<uint16_t>(Effect_Blend::Target::TARGET1_BACK) | static_cast<uint16_t>(Effect_Blend::Mode::MODE_ALPHA);
    REG_BLDALPHA = (16 - ey) << 8 | ey;
}

void prepareAffines(Data & data)
{
    if (data.horizon >= SCREEN_HEIGHT)
    {
        return;
    }
    const int32_t horizon = (data.horizon >= 0 ? data.horizon : 0);
    const Camera * cam = data.camera;
    FIXED yc = cam->pos.y;

    Effect_Affine::AffineData * affine = &data.affineData[horizon];
    FIXED yb, zb;         // b' = Rx(theta) *  (L, ys, -D)
    FIXED lam, lcf, lsf;  // scale and scaled (co)sine(phi)
    Math::fp1616_t cp = cam->u.x;
    Math::fp1616_t sp = cam->u.z;
    Math::fp1616_t ct = cam->v.y;
    Math::fp1616_t st = cam->w.y;
    for (int32_t screenLine = horizon; screenLine < SCREEN_HEIGHT; screenLine++)
    {
        yb = (screenLine - M7_TOP) * ct + M7_D * st;
        lam = DivSafe(yc << 12, yb); // .12f

        lcf = lam * cp >> 8; // .12f
        lsf = lam * sp >> 8; // .12f

        bga->pa = lcf >> 4; // .8f
        bga->pc = lsf >> 4; // .8f

        // lambda·Rx·b
        zb = (screenLine - M7_TOP) * st - M7_D * ct;                    // .8f
        bga->dx = cam->pos.x + (lcf >> 4) * M7_LEFT - (lsf * zb >> 12); // .8f
        bga->dy = cam->pos.z + (lsf >> 4) * M7_LEFT + (lcf * zb >> 12); // .8f

        // hack that I need for fog. pb and pd are unused anyway
        bga->pb = lam;
        bga++;
    }
    data.bgaff[SCREEN_HEIGHT] = data.bgaff[0];
}

//! Setup an object's attr/affine with the right attributes
/*! \param level	Mode 7 level data
*	\param spr		3D sprite to calculate for
*/
void m7_prep_sprite(M7_LEVEL *level, M7_SPRITE *spr)
{
    M7_CAM *cam = data.camera;
    VECTOR vr, vc; // Difference and inverted-cam vector
    int sx, sy;    // Object size
    RECT rect;     // Object rectangle

    // Convert to camera frame
    vec_sub(&vr, &spr->pos, &cam->pos);
    vc.x = vec_dot(&vr, &cam->u);
    vc.y = -vec_dot(&vr, &cam->v);
    vc.z = -vec_dot(&vr, &cam->w);
    spr->pos2 = vc;

    OBJ_ATTR *obj = &spr->obj;
    sx = obj_get_width(obj);
    sy = obj_get_height(obj);

    // --- Check with viewbox ---
    do
    {
        if (M7_NEAR * 256 > vc.z || vc.z > M7_FAR * 256)
            break;

        rect.left = vc.x - spr->anchor.x * (256 >> M7O_NORM);
        rect.right = rect.left + sx * (256 >> M7O_NORM);

        if (M7_LEFT * vc.z > rect.right * M7_D || rect.left * M7_D > M7_RIGHT * vc.z)
            break;

        rect.top = vc.y - spr->anchor.y * (256 >> M7O_NORM);
        rect.bottom = rect.top + sy * (256 >> M7O_NORM);

        if (-M7_TOP * vc.z > rect.bottom * M7_D || rect.top * M7_D > -M7_BOTTOM * vc.z)
            break;

        OBJ_AFFINE *oa = &obj_aff_mem[spr->aff_id];
        oa->pa = oa->pd = vc.z >> (M7_D_SHIFT - M7O_NORM); // normalized lambda
        oa->pb = oa->pc = 0;

        FIXED scale = DivSafe(M7_D << 16, vc.z); // (.16 / .8) = .8

        // Base anchoring equation:
        // x = q0 - s - A(p0 - s/2)
        // In this case A = 1/lam; and q0 = xc/lam
        // -> x = (xc - p0 + s/2)/lam - s + screen/2
        int xscr, yscr;
        xscr = spr->anchor.x * 256 - sx * 128;            // .8
        xscr = (vc.x - (xscr >> M7O_NORM)) * scale >> 16; // .0
        xscr += -sx - M7_LEFT;

        yscr = spr->anchor.y * 256 - sy * 128;            // .8
        yscr = (vc.y - (yscr >> M7O_NORM)) * scale >> 16; // .0
        yscr += -sy + M7_TOP;

        obj_unhide(obj, ATTR0_AFF_DBL);
        obj_set_pos(obj, xscr, yscr);

        return;
    } while (0);

    obj_hide(obj);

    // TODO: add shadow
}

} // namespace Mode7