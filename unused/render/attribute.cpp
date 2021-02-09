#include "attribute.h"

#ifdef TARGET_PC
#include <cstring>
#include <map>
#else
#include "memory/memory.h"
#endif
#include <utility>

namespace Render
{

#ifdef TARGET_PC
struct AttributeRoleInfo
{
    std::string name;
    bool mustNormalize;
    ComponentType preferredType;
};

struct ComponentTypeInfo
{
    std::string typeName;
    std::pair<double, double> allowedRange;
};

const std::map<Attribute::Role, AttributeRoleInfo> RoleInfoMap = {
    {Attribute::Role::BAD_ROLE, {"BAD_ROLE", false, ComponentType::BAD_TYPE}},
    {Attribute::Role::INDEX, {"INDEX", false, ComponentType::U16}},
    {Attribute::Role::POSITION, {"POSITION", false, ComponentType::FP1616}},
    {Attribute::Role::NORMAL, {"NORMAL", true, ComponentType::FP412}},
    {Attribute::Role::TANGENT, {"TANGENT", true, ComponentType::FP412}},
    {Attribute::Role::COLOR_0, {"COLOR_0", false, ComponentType::U8}},
    {Attribute::Role::TEXCOORD_0, {"TEXCOORD_0", true, ComponentType::FP412}},
    {Attribute::Role::TEXCOORD_1, {"TEXCOORD_1", true, ComponentType::FP412}},
    {Attribute::Role::ARBITRARY_0, {"ARBITRARY_0", false, ComponentType::FP1616}},
    {Attribute::Role::ARBITRARY_1, {"ARBITRARY_1", false, ComponentType::FP1616}},
    {Attribute::Role::FACE_CENTER, {"FACE_CENTER", false, ComponentType::FP1616}},
    {Attribute::Role::FACE_NORMAL, {"FACE_NORMAL", false, ComponentType::FP412}}};

const std::map<ComponentType, ComponentTypeInfo> ComponentTypeInfoMap = {
    {ComponentType::BAD_TYPE, {"bad type", {0.0, 0.0}}},
    {ComponentType::U8, {"uint8_t", {0.0, 255.0}}},
    {ComponentType::U16, {"uint16_t", {0.0, 65535.0}}},
    {ComponentType::U32, {"uint32_t", {0.0, 4294967295.0}}},
    {ComponentType::FP1616, {"fp1616_t", {-32767.0, 32767.0}}},
    {ComponentType::FP412, {"fp412_t", {-7.0, 7.0}}}};

Attribute::Role Attribute::matchNameToRole(const std::string &roleName)
{
    for (const auto &e : RoleInfoMap)
    {
        if (e.second.name == roleName)
        {
            return e.first;
        }
    }
    return Attribute::Role::BAD_ROLE;
}

ComponentType Attribute::preferredComponentTypeFor(Attribute::Role role)
{
    return RoleInfoMap.at(role).preferredType;
}

std::pair<double, double> Attribute::allowedRangeFor(ComponentType type)
{
    return ComponentTypeInfoMap.at(type).allowedRange;
}

bool Attribute::mustNormalizeComponentsFor(Attribute::Role role)
{
    return RoleInfoMap.at(role).mustNormalize;
}

std::string Attribute::toString(Role role)
{
    return RoleInfoMap.at(role).name;
}

std::string Attribute::toString(ComponentType type)
{
    return ComponentTypeInfoMap.at(type).typeName;
}
#endif

Attribute::Attribute()
{
}

Attribute::Attribute(Attribute &&other) noexcept
{
    *this = std::move(other);
}

Attribute::Attribute(const uint8_t *data, uint16_t count, uint16_t nrOfComponents, ComponentType componentType, Role role, CopyPolicy policy)
{
    if (role != Role::BAD_ROLE && componentType != ComponentType::BAD_TYPE && nrOfComponents > 0)
    {
        // free data in this
        free();
        // set members
        m_data = const_cast<uint8_t *>(data);
        m_count = count;
        m_nrOfComponents = nrOfComponents;
        m_componentType = componentType;
        m_role = role;
        // allocate data if wanted by caller
        if (policy == CopyPolicy::ALLOCATE_DATA || policy == CopyPolicy::ALLOCATE_AND_COPY_DATA)
        {
            // calculate data size
            auto dataSize = GetBytesPerComponent(m_componentType) * static_cast<uint32_t>(m_nrOfComponents) * static_cast<uint32_t>(m_count);
#ifdef TARGET_PC
            m_data = new uint8_t[dataSize];
            if (policy == CopyPolicy::ALLOCATE_AND_COPY_DATA)
            {
                std::memcpy(m_data, data, dataSize);
            }
#else
            // round data size to multiple of 4
            dataSize = (dataSize + 3) & (~0x03);
            m_data = Memory::malloc_EWRAM<uint8_t>(dataSize);
            if (policy == CopyPolicy::ALLOCATE_AND_COPY_DATA)
            {
                Memory::memcpy32(m_data, data, dataSize / 4);
            }
#endif
            m_allocated = 1;
        }
    }
}

Attribute &Attribute::operator=(Attribute &&other) noexcept
{
    // free data in this
    free();
    // copy members over
    m_data = other.m_data;
    m_count = other.m_count;
    m_nrOfComponents = other.m_nrOfComponents;
    m_componentType = other.m_componentType;
    m_role = other.m_role;
    m_allocated = other.m_allocated;
    // invalidate other
    other.m_data = nullptr;
    other.m_count = 0;
    other.m_nrOfComponents = 0;
    other.m_componentType = ComponentType::BAD_TYPE;
    other.m_role = Role::BAD_ROLE;
    other.m_allocated = 0;
    return *this;
}

Attribute::~Attribute()
{
    free();
}

Attribute::Role Attribute::role() const
{
    return m_role;
}

uint16_t Attribute::count() const
{
    return m_count;
}

uint16_t Attribute::nrOfComponents() const
{
    return m_nrOfComponents;
}

ComponentType Attribute::componentType() const
{
    return m_componentType;
}

void Attribute::free()
{
    if (m_allocated != 0)
    {
#ifdef TARGET_PC
        delete[] m_data;
#else
        Memory::free(m_data);
#endif
        m_allocated = 0;
    }
    m_data = nullptr;
    m_count = 0;
    m_nrOfComponents = 0;
    m_componentType = ComponentType::BAD_TYPE;
    m_role = Role::BAD_ROLE;
}

} // namespace Render
