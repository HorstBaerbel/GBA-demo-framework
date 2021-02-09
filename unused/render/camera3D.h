#pragma once

#include "math/fp32.h"
#include "math/vec.h"
#include "math/quat.h"
#include "math/mat.h"

namespace Render
{

/// @brief General 3D camera.
struct Camera3D
{
    /// @brief Get current camera position.
    const Math::fp1616vec3_t &position() const;
    /// @brief Set new camera position.
    void setPosition(const Math::fp1616vec3_t &position);
    /// @brief Set new camera orientation.
    void setOrientation(const Math::fp1616quat_t &orientation);
    /// @brief Get view matrix. Will recalculate it if necessary.
    const Math::fp1616mat3x4_t &viewMatrix();

    void setProjection(Math::fp1616_t fov, Math::fp1616_t near, Math::fp1616_t far);
    /// @brief Get projection matrix. Will recalculate it if necessary.
    const Math::fp1616mat4x4_t &projectionMatrix();

private:
    Math::fp1616vec3_t m_position = {0, 0, 0};       //!< World position
    Math::fp1616quat_t m_orientation = {0, 0, 0, 1}; //!< World orientation
    Math::fp1616mat3x4_t m_view;                     //!< View matrix
    bool m_viewDirty = true;                         //!< If true, view matrix will be recalculated on next viewMatrix() call
    Math::fp1616_t m_fov = 90;                       //!< Field of view
    Math::fp1616_t m_nearZ = 1;                      //!< Near clipping plane
    Math::fp1616_t m_farZ = 100;                     //!< Far clipping plane
    Math::fp1616mat4x4_t m_projection;               //!< Projection matrix
    bool m_projectionDirty = true;                   //!< If true, projection matrix will be recalculated on next projectionMatrix() call
};

} // namespace Render