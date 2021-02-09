#pragma once

#include "attribute.h"
#ifdef TARGET_PC
#include <string>
#endif
#include <stdint.h>

namespace Render
{

	struct Mesh
	{

		enum PrimitiveMode : uint8_t
		{
			BAD_MODE = 0,
			POINTS = 1,
			LINES = 2,
			TRIANGLES = 3
		};

		/// @brief Maximum number of attributes per mesh.
		static constexpr uint32_t MAX_NR_OF_ATTRIBUTES = 8;

		/// @brief Default constructor.
		Mesh();

		/// @brief Move constructor.
		Mesh(Mesh &&other) noexcept;

		/// @brief Move assigment.
		Mesh &operator=(Mesh &&other) noexcept;

		/// @brief Set the render mode of the mesh.
		void setPrimitiveMode(PrimitiveMode mode);
		/// @brief Get the render mode of the mesh.
		PrimitiveMode primitiveMode() const;

		/// @brief Move constant or modifiable attribute buffer into mesh.
		/// @param attribute Attribute to move into buffer. If it is moved attribute will be undefined afterwards.
		void addAttribute(Attribute &&attribute) noexcept;

		/// @brief Get index'th attribute.
		/// @param index Index of attribute to return.
		/// @return Index'th attribute or undefined behaviour if no attributes or bad index given.
		Attribute &attribute(uint16_t index);

		/// @brief Get index'th attribute.
		/// @param index Index of attribute to return.
		/// @return Index'th attribute or undefined behaviour if no attributes or bad index given.
		const Attribute &attribute(uint16_t index) const;

		/// @brief Get attribute with specific role.
		/// @param role Role of attribute to return.
		/// @return Attribute with role or nullptr if no attributes found or bad role given.
		Attribute *findAttributeWithRole(Attribute::Role role);

		/// @brief Get attribute with specific role.
		/// @param role Role of attribute to return.
		/// @return Attribute with role or nullptr if no attributes found or bad role given.
		const Attribute *findAttributeWithRole(Attribute::Role role) const;

		/// @brief Get the number of attributes in the mesh.
		/// @return Number of attributes in the mesh.
		uint16_t count() const;

		/// @brief Free all allocated attribute and index data.
		void free();

#ifdef TARGET_PC
		/// @brief Convert PrimitveMode to string.
		static std::string toString(PrimitiveMode mode);
#endif

	private:
		/// @brief Not copyable!
		Mesh(const Mesh &other);
		/// @brief Not copyable!
		Mesh &operator=(const Mesh &other);

		Attribute m_attributes[MAX_NR_OF_ATTRIBUTES];
		uint16_t m_nrOfAttributes = 0;
		PrimitiveMode m_primitiveMode = PrimitiveMode::POINTS;
		uint8_t _dummy; // for alignment
	} __attribute__((aligned(4), packed));

} // namespace Render
