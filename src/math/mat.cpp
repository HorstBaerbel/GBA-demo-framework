#include "mat.h"

namespace Math
{

        template <>
        const mat2x2_t<Math::fp1616_t> mat2x2_t<Math::fp1616_t>::identity = {{1, 0}, {0, 1}};
        template <>
        const mat2x2_t<Math::fp1616_t> mat2x2_t<Math::fp1616_t>::zero = {{0, 0}, {0, 0}};

        template <>
        const mat3x3_t<Math::fp1616_t> mat3x3_t<Math::fp1616_t>::identity = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        template <>
        const mat3x3_t<Math::fp1616_t> mat3x3_t<Math::fp1616_t>::zero = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

        template <>
        const mat3x4_t<Math::fp1616_t> mat3x4_t<Math::fp1616_t>::identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}};
        template <>
        const mat3x4_t<Math::fp1616_t> mat3x4_t<Math::fp1616_t>::zero = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

        template <>
        const mat4x4_t<Math::fp1616_t> mat4x4_t<Math::fp1616_t>::identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
        template <>
        const mat4x4_t<Math::fp1616_t> mat4x4_t<Math::fp1616_t>::zero = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

}
