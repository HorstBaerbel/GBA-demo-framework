#include "camera.h"

namespace Camera
{

    void update(C8DOF &camera)
    {
        Math::fp1616_t sp, cp;
        sincos(camera.phi, sp, cp);
        // camera x-axis (right)
        camera.u.x = cp;
        camera.u.y = 0;
        camera.u.z = -sp;
        // clamp to some sane pitch angle
        camera.theta = clamp(camera.theta, -Math::fp1616_t::PI_HALF, Math::fp1616_t::PI_HALF);
        Math::fp1616_t st, ct;
        sincos(camera.theta, st, ct);
        // camera y-axis (up)
        camera.v.x = sp * st;
        camera.v.y = cp;
        camera.v.z = sp * ct;
        // camera z-axis (back)
        camera.w.x = cp * ct;
        camera.w.y = -sp;
        camera.w.z = cp * ct;
    }

    void setOrientation(C8DOF &camera, Math::fp1616_t phi, Math::fp1616_t theta)
    {
        // clamp to some sane pitch angle
        theta = clamp(theta, -Math::fp1616_t::PI_QUARTER, Math::fp1616_t::PI_QUARTER);
        camera.phi = phi;
        camera.theta = theta;
        update(camera);
    }

    void translateLevel(C8DOF &camera, const Math::fp1616vec3_t &dir)
    {
        camera.position.x += camera.u.x * dir.x - camera.u.z * dir.z;
        camera.position.y += dir.y;
        camera.position.z += camera.u.z * dir.x + camera.u.x * dir.z;
    }

    void translateLocal(C8DOF &camera, const Math::fp1616vec3_t &dir)
    {
        camera.position.x += camera.u.x * dir.x + camera.v.x * dir.y + camera.w.x * dir.z;
        camera.position.y += camera.v.y * dir.y + camera.w.y * dir.z;
        camera.position.z += camera.u.z * dir.x + camera.v.z * dir.y + camera.w.z * dir.z;
    }

} // namespace Camera