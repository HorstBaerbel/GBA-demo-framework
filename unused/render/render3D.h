#pragma once

#include "vertexdata.h"

namespace Render
{
// * calculate camera transform
// * calculate object transform
// * do object-space backface culling:
// calculate model-view matrix by multiplying object and camera transform
// invert matrix to get camera to object space transform
// transform camera position using matrix
// per polygon do:
// calculate direction camera->polygon using one of the polygon vertices: dc = p0 - pc
// get face normal n from data or calculate from polygon
// calculate dot product of direction and face normal: a = dc.n
// if a <= 0 the polygon is front facing, else cull it.
// * do clipping:
// calculate transform to clip space by multiplying model-view matrix and perspective transform
// per polygon do:
// transform polygon points to clip space or read from / write to cache
// cull polygon if all points in front of view plane / behind camera
// clip polygon points. generates 1-2 new polygons
// interpolate colors, texture coordinates etc. accordingly. write to cache
// * draw polygon(s)
void renderTriangles();

} // namespace Render
