#pragma once

#include "common.h"

#include <stdint.h>
#ifdef TARGET_PC
#include <string>
#endif

namespace Render
{

	using VertexId_t = uint16_t;

	/// @brief Can store either source / constant or generated / transformed interleaved vertex attribute data
	/// Initialize with either const source data (data != nullptr, count > 0),
	/// or allocate memory in EWRAM with (data == nullptr, count > 0).
	struct Attribute
	{

		/// @brief Attribute roles supported for renderer
		enum class Role : uint8_t
		{
			BAD_ROLE = 0,
			INDEX = 1,		  // Index into arrays
			POSITION = 2,	  // Position of point / line / triangle
			NORMAL = 3,		  // Vertex normal
			TANGENT = 4,	  // Vertex normal
			COLOR_0 = 5,	  // Vertex color
			TEXCOORD_0 = 6,	  // Vertex texture coordinate #0
			TEXCOORD_1 = 7,	  // Vertex texture coordinate #1
			ARBITRARY_0 = 8,  // Multi-purpose attribute #0
			ARBITRARY_1 = 9,  // Multi-purpose attribute #1
			FACE_CENTER = 10, // Center position of face / polygon (for sorting and backface culling)
			FACE_NORMAL = 11  // Normal of face / polygon (for backface culling and flat shading)
		};

#ifdef TARGET_PC
		static Role matchNameToRole(const std::string &roleName);
		static bool mustNormalizeComponentsFor(Role role);
		static ComponentType preferredComponentTypeFor(Role role);
		static std::pair<double, double> allowedRangeFor(ComponentType outputType);
		static std::string toString(Role role);
		static std::string toString(ComponentType type);
#endif

		/// @brief Default constructor.
		Attribute();

		/// @brief Move constructor.
		Attribute(Attribute &&other) noexcept;

		/// @brief Will store constant or allocate and copy data for attributes.
		/// @param data Pointer to raw data.
		/// @param count Number of elements (scalar, vec2, vec3) in attribute.
		/// @param nrOfComponents Number of components in attribute (e.g. 3 for vec3).
		/// @param componentType Type of a single component (e.g. FP1616 for a position vec3).
		/// @param role Attribute role.
		/// @param policy What to do with the data passed.
		Attribute(const uint8_t *data, uint16_t count, uint16_t nrOfComponents, ComponentType componentType, Role role, CopyPolicy policy);

		/// @brief Move assigment.
		Attribute &operator=(Attribute &&other) noexcept;

		/// @brief Destructor.
		~Attribute();

		/// @brief Get role for attribute.
		Role role() const;

		/// @brief Get number of elements for attribute.
		uint16_t count() const;

		/// @brief Get number of components for attribute.
		uint16_t nrOfComponents() const;

		/// @brief Get type info for attribute or index.
		ComponentType componentType() const;

		/// @brief Free all allocated attribute data.
		void free();

		/// @brief Get const pointer to attribute data.
		template <typename T>
		const T *data() const
		{
			return reinterpret_cast<const T *>(m_data);
		}

		/// @brief Get pointer to attribute data.
		template <typename T>
		T *data()
		{
			return reinterpret_cast<T *>(m_data);
		}

	private:
		/// @brief Not copyable!
		Attribute(const Attribute &other);
		/// @brief Not copyable!
		Attribute &operator=(const Attribute &other);

		uint8_t *m_data = nullptr;								 // raw pointer to data
		uint16_t m_count = 0;									 // number of elements of attribute type. must be > 0
		uint16_t m_nrOfComponents = 0;							 // number of components per element of attribute type. must be > 0
		ComponentType m_componentType = ComponentType::BAD_TYPE; // component type of attribute
		Role m_role = Role::BAD_ROLE;							 // role of attribute
		uint8_t m_allocated = 0;								 // internal. != 0 if this object allocated the data
		uint8_t _dummy;											 // for alignment
	} __attribute__((aligned(4), packed));

} // namespace Render
