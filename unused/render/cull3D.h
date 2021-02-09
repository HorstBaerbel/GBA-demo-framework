#pragma once

#include "vertexdata.h"

namespace Render
{

void cullBackface(const VertexData &srcData, VertexData &dstData);

}
