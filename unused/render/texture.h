#pragma once

#include "common.h"

#include <cstdint>

namespace Render
{

    using TextureId_t = uint16_t;

    // Texture class. Holds raw memory, either just a pointer to const memory or allocated memory.
    // Textures must be power-of-two and can only have U8, U16 or U32 components to be usable.
    struct Texture
    {

        /// @brief Default constructor.
        Texture();

        /// @brief Move constructor.
        Texture(Texture &&other) noexcept;

        /// @brief Will store constant or allocate and copy data for textures.
        /// @param data Pointer to raw data.
        /// @param width Power-of-two width of texture.
        /// @param height Power-of-two height of texture.
        /// @param componentType Type of a single component (e.g. u8 for an indexed color).
        /// @param policy What to do with the data passed.
        Texture(const uint8_t *data, Pow2Size width, Pow2Size height, ComponentType componentType, CopyPolicy policy);

        /// @brief Move assigment.
        Texture &operator=(Texture &&other) noexcept;

        /// @brief Destructor.
        ~Texture();

        /// @brief Get the width of the texture.
        /// @return Width of the texture.
        Pow2Size width() const;

        /// @brief Get the height of the texture.
        /// @return Height of the texture.
        Pow2Size height() const;

        /// @brief Get type info for pixel data.
        ComponentType componentType() const;

        /// @brief Free all allocated texture data.
        void free();

        /// @brief Get const pointer to pixel data.
        template <typename T>
        const T *data() const
        {
            return reinterpret_cast<const T *>(m_data);
        }

        /// @brief Get pointer to pixel data.
        template <typename T>
        T *data()
        {
            return reinterpret_cast<T *>(m_data);
        }

    private:
        /// @brief Not copyable!
        Texture(const Texture &other);
        /// @brief Not copyable!
        Texture &operator=(const Texture &other);

        uint8_t *m_data = nullptr;                               // raw pointer to data
        Pow2Size m_width = Pow2Size::BAD_SIZE;                   // width of pixel data
        Pow2Size m_height = Pow2Size::BAD_SIZE;                  // height of pixel data
        ComponentType m_componentType = ComponentType::BAD_TYPE; // component type of pixel data
        uint8_t m_allocated = 0;                                 // internal. != 0 if this object allocated the data.
        uint16_t _dummy;                                         // for alignment
    } __attribute__((aligned(4), packed));

} // namespace Render
