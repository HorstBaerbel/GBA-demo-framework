#pragma once

#include <gba_base.h>
#include "camera.h"
#include "effect/affine.h"
#include "sprites.h"
#include "vfp32.h"

namespace Mode7
{

static constexpr uint16_t FOCAL_LENGTH = 256; //!< D-Parameter.
static constexpr uint16_t FOCAL_SHIFT = 8; //!< D-Parameter shift (1/256).
static constexpr uint16_t OBJ_RENORM_SHIFT = 2; //!< Object renormalization shift (1/4).
static constexpr int16_t VIEWPORT_LEFT = -120; //!< Viewport left.
static constexpr int16_t VIEWPORT_RIGHT = 120; //!< Viewport left.
static constexpr int16_t VIEWPORT_TOP = 80; //!< Viewport top (y-axis up).
static constexpr int16_t VIEWPORT_BOTTOM = 80; //!< Viewport bottom (y-axis up).
static constexpr int16_t VIEWPORT_FLOOR_FAR = 768; //!< Viewport far plane for floor.
static constexpr int16_t VIEWPORT_OBJ_NEAR = 768; //!< Viewport near plane for objects.
static constexpr int16_t VIEWPORT_OBJ_FAR = 768; //!< Viewport far plane for objects.

struct Sprite3D
{
    Math::fp1616vec3_t position; //!< World position.
    Math::fp1616vec2_t anchor; //!< Sprite anchor.
    Math::fp1616_t phi; //!< Azimuth angle.
    Math::fp1616vec2_t camPos; //!< Position in camera space.
    Sprites::Sprite2D sprite; //!< 2D sprite attributes and data.
};

struct Data
{
    Camera::C8DOF *camera; //!< Camera.
    Effect_Affine::AffineData *affineData; //!< Data for the affine BG registers
    Sprite3D *sprites; //!< Sprites data.
    int16_t horizon; //!< Horizon scanline.
    uint16_t bgcntBackdrop; //!< BGxCNT for backdrop.
    uint16_t bgcntFloor; //!< BGxCNT for floor.
};
extern Data data;

void init(Data &data, Camera::C8DOF *camera, Effect_Affine::AffineData *affineData, Sprite3D *sprites, uint16_t bgcntBackdrop, uint16_t bgcntFloor);
void prepareHorizon(Data &data);
void updateHorizon(const Data &data);

inline int16_t horizonLine(const Data &data) { return data.horizon < 0 ? 0 : (data.horizon > 228 ? 228 : data.horizon); }

void prepareAffines(Data &data);
void prepareSprite(Data &data, Sprite3D *sprites);
void hblankFloor();

} // namespace Mode7