#pragma once

#include "math/vec.h"
#include "math/mat.h"
#include "mesh.h"

namespace Render
{

/// @brief Stores object information using an index and vertex buffer.
struct Object3D
{
    /// @brief Get current object position.
    const Math::fp1616vec3_t &position() const;
    /// @brief Set new object position.
    void setPosition(const Math::fp1616vec3_t &position);
    /// @brief Set new object orientation.
    void setOrientation(const Math::fp1616quat_t &orientation);

    /// @brief Get model matrix. Will recalculate it if necessary.
    const Math::fp1616mat3x4_t &modelMatrix();

private:
    Mesh m_mesh;
    Math::fp1616vec3_t m_position = {0, 0, 0};       //!< World position
    Math::fp1616quat_t m_orientation = {0, 0, 0, 1}; //!< World orientation
    Math::fp1616mat3x4_t m_model;
    bool m_modelDirty = true; //!< If true, model matrix will be recalculate on next modelMatrix() call
    TextureId_t m_textureId = 0;
};

} // namespace Render
