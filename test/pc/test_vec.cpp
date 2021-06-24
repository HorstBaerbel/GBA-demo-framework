#include <debug/print.h>
//#include <random.h>
#include <math/vec.h>

//disable GCC warnings for using char * here...
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace Test
{

    using namespace Math;

    void math_vec()
    {
        printf("Vector types...\nvec2:\n");
        printf("Size of fp412vec2_t: %d\n", (uint32_t)sizeof(fp412vec2_t));
        printf("Size of fp88vec2_t: %d\n", (uint32_t)sizeof(fp88vec2_t));
        printf("Size of fp124vec2_t: %d\n", (uint32_t)sizeof(fp124vec2_t));
        printf("Size of fp824vec2_t: %d\n", (uint32_t)sizeof(fp824vec2_t));
        printf("Size of fp1616vec2_t: %d\n", (uint32_t)sizeof(fp1616vec2_t));
        printf("Size of fp248vec2_t: %d\n", (uint32_t)sizeof(fp248vec2_t));
        printf("vec3:\n");
        printf("Size of fp412vec3_t: %d\n", (uint32_t)sizeof(fp412vec3_t));
        printf("Size of fp88vec3_t: %d\n", (uint32_t)sizeof(fp88vec3_t));
        printf("Size of fp124vec3_t: %d\n", (uint32_t)sizeof(fp124vec3_t));
        printf("Size of fp824vec3_t: %d\n", (uint32_t)sizeof(fp824vec3_t));
        printf("Size of fp1616vec3_t: %d\n", (uint32_t)sizeof(fp1616vec3_t));
        printf("Size of fp248vec3_t: %d\n", (uint32_t)sizeof(fp248vec3_t));
        printf("vec4:\n");
        printf("Size of fp412vec4_t: %d\n", (uint32_t)sizeof(fp412vec4_t));
        printf("Size of fp88vec4_t: %d\n", (uint32_t)sizeof(fp88vec4_t));
        printf("Size of fp124vec4_t: %d\n", (uint32_t)sizeof(fp124vec4_t));
        printf("Size of fp824vec4_t: %d\n", (uint32_t)sizeof(fp824vec4_t));
        printf("Size of fp1616vec4_t: %d\n", (uint32_t)sizeof(fp1616vec4_t));
        printf("Size of fp248vec4_t: %d\n", (uint32_t)sizeof(fp248vec4_t));
    }

} // namespace Test