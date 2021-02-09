#include "camera3D.h"

namespace Render
{

const Math::fp1616vec3_t &Camera3D::position() const
{
    return m_position;
}

void Camera3D::setPosition(const Math::fp1616vec3_t &position)
{
    if (m_position != position)
    {
        m_position = position;
        m_viewDirty = true;
    }
}

void Camera3D::setOrientation(const Math::fp1616quat_t &orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        m_viewDirty = true;
    }
}

const Math::fp1616mat3x4_t &Camera3D::viewMatrix()
{
    if (m_viewDirty)
    {
        m_viewDirty = false;
        m_view = Math::fp1616mat3x4_t::fromTranslate(m_position) * Math::fp1616mat3x4_t::fromRotate(m_orientation.toRotation());
    }
    return m_view;
}

void Camera3D::setProjection(Math::fp1616_t fov, Math::fp1616_t nearZ, Math::fp1616_t farZ)
{
    if (m_fov != fov || m_nearZ != nearZ || m_farZ != farZ)
    {
        m_projectionDirty = true;
        m_fov = fov;
        m_nearZ = nearZ;
        m_farZ = farZ;
    }
}

const Math::fp1616mat4x4_t &Camera3D::projectionMatrix()
{
    if (m_projectionDirty)
    {
        m_projectionDirty = false;
        m_projection.setZero();
        auto scale = Math::fp1616_t::ONE / Math::tan(m_fov * Math::fp1616_t::PI / 360);
        m_projection[0][0] = scale;                                  // scale the x coordinates of the projected point
        m_projection[1][1] = scale;                                  // scale the y coordinates of the projected point
        m_projection[2][2] = -m_farZ / (m_farZ - m_nearZ);           // used to remap z to [0,1]
        m_projection[3][2] = -m_farZ * m_nearZ / (m_farZ - m_nearZ); // used to remap z [0,1]
        m_projection[2][3] = -Math::fp1616_t::ONE;                   // set w = -z
    }
    return m_projection;
}

} // namespace Render