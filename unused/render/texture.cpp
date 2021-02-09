#include "texture.h"

#ifdef TARGET_PC
#include <map>
#include <cstring>
#else
#include "memory/memory.h"
#endif
#include <utility>

namespace Render
{

    Texture::Texture()
    {
    }

    Texture::Texture(Texture &&other) noexcept
    {
        *this = std::move(other);
    }

    Texture::Texture(const uint8_t *data, Pow2Size width, Pow2Size height, ComponentType componentType, CopyPolicy policy)
    {
        if (width != Pow2Size::BAD_SIZE && height != Pow2Size::BAD_SIZE && componentType != ComponentType::BAD_TYPE)
        {
            // free data in this
            free();
            // set members
            m_data = const_cast<uint8_t *>(data);
            m_width = width;
            m_height = height;
            m_componentType = componentType;
            // allocate data if wanted by caller
            if (policy == CopyPolicy::ALLOCATE_DATA || policy == CopyPolicy::ALLOCATE_AND_COPY_DATA)
            {
                // calculate data size
                auto dataSize = GetBytesPerComponent(m_componentType) * static_cast<uint32_t>(m_width) * static_cast<uint32_t>(m_height);
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

    Texture &Texture::operator=(Texture &&other) noexcept
    {
        // free data in this
        free();
        // copy members over
        m_data = other.m_data;
        m_width = other.m_width;
        m_height = other.m_height;
        m_componentType = other.m_componentType;
        m_allocated = other.m_allocated;
        // invalidate other
        other.m_data = nullptr;
        other.m_width = Pow2Size::BAD_SIZE;
        other.m_height = Pow2Size::BAD_SIZE;
        other.m_componentType = ComponentType::BAD_TYPE;
        other.m_allocated = 0;
        return *this;
    }

    Texture::~Texture()
    {
        free();
    }

    Pow2Size Texture::width() const
    {
        return m_width;
    }

    Pow2Size Texture::height() const
    {
        return m_height;
    }

    ComponentType Texture::componentType() const
    {
        return m_componentType;
    }

    void Texture::free()
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
        m_width = Pow2Size::BAD_SIZE;
        m_height = Pow2Size::BAD_SIZE;
        m_componentType = ComponentType::BAD_TYPE;
    }

} // namespace Render
