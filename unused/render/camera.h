#pragma once

#include "math/fp32.h"
#include "math/vec.h"
#include "math/quat.h"

namespace Camera
{

    /// @brief Camera that has only yaw and pitch, but no roll. Can be used e.g. for Mode7.
    /// @note Rigth-handed coordinate system
    struct C8DOF
    {
        Math::fp1616vec3_t position; //!< World position.
        Math::fp1616_t phi;          //!< Polar angle. Yaw.
        Math::fp1616_t theta;        //!< Azimuth angle. Pitch.
        Math::fp1616vec3_t u;        //!< Local x-axis (right).
        Math::fp1616vec3_t v;        //!< Local y-axis (up).
        Math::fp1616vec3_t w;        //!< Local z-axis (back).
        Math::fp1616_t nearZ;        //!< Camera near plane.
        Math::fp1616_t farZ;         //!< Camera far plane.
    };

    /// @brief Update camera coordinate system from current parameters.
    void update(C8DOF &camera);
    /// @brief Set new rotation in camera and update.
    void setOrientation(C8DOF &camera, Math::fp1616_t theta, Math::fp1616_t phi);
    /// @brief Translate camera in local frame, but with global z.
    void translateLevel(C8DOF &camera, const Math::fp1616vec3_t &dir);
    /// @brief Translate camera in local frame.
    void translateLocal(C8DOF &camera, const Math::fp1616vec3_t &dir);

} // namespace Camera