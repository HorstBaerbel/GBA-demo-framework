#include "mesh.h"

#ifdef TARGET_PC
#include <map>
#endif
#include <utility>

namespace Render
{

#ifdef TARGET_PC
	const std::map<Mesh::PrimitiveMode, std::string> PrimitiveModeMap = {
		{Mesh::PrimitiveMode::BAD_MODE, "bad mode"},
		{Mesh::PrimitiveMode::POINTS, "points"},
		{Mesh::PrimitiveMode::LINES, "lines"},
		{Mesh::PrimitiveMode::TRIANGLES, "triangles"}};

	std::string Mesh::toString(PrimitiveMode mode)
	{
		return PrimitiveModeMap.at(mode);
	}
#endif

	Mesh::Mesh()
	{
	}

	Mesh::Mesh(Mesh &&other) noexcept
	{
		*this = std::move(other);
	}

	Mesh &Mesh::operator=(Mesh &&other) noexcept
	{
		// free data in this
		free();
		// copy members over
		for (unsigned int i = 0; i < other.m_nrOfAttributes; ++i)
		{
			m_attributes[i] = std::move(other.m_attributes[i]);
		}
		m_nrOfAttributes = other.m_nrOfAttributes;
		m_primitiveMode = other.m_primitiveMode;
		// invalidate other
		other.m_nrOfAttributes = 0;
		other.m_primitiveMode = PrimitiveMode::BAD_MODE;
		return *this;
	}

	void Mesh::setPrimitiveMode(PrimitiveMode type)
	{
		m_primitiveMode = type;
	}

	Mesh::PrimitiveMode Mesh::primitiveMode() const
	{
		return m_primitiveMode;
	}

	Attribute *Mesh::findAttributeWithRole(Attribute::Role role)
	{
		for (uint32_t i = 0; i < m_nrOfAttributes; ++i)
		{
			if (m_attributes[i].role() == role)
			{
				return &m_attributes[i];
			}
		}
		return nullptr;
	}

	const Attribute *Mesh::findAttributeWithRole(Attribute::Role role) const
	{
		for (uint32_t i = 0; i < m_nrOfAttributes; ++i)
		{
			if (m_attributes[i].role() == role)
			{
				return &m_attributes[i];
			}
		}
		return nullptr;
	}

	void Mesh::addAttribute(Attribute &&attribute) noexcept
	{
		// search for attribute with that role
		auto dst = findAttributeWithRole(attribute.role());
		if (dst)
		{
			// if found, free destination first
			dst->free();
		}
		if (m_nrOfAttributes < (MAX_NR_OF_ATTRIBUTES - 1))
		{
			// if no existing found, find first free one
			dst = &m_attributes[m_nrOfAttributes];
			m_nrOfAttributes++;
		}
		if (dst)
		{
			// if we've found a place to move the attribute do it and invalidate the old one
			*dst = std::move(attribute);
		}
	}

	Attribute &Mesh::attribute(uint16_t index)
	{
		return m_attributes[index];
	}

	const Attribute &Mesh::attribute(uint16_t index) const
	{
		return m_attributes[index];
	}

	uint16_t Mesh::count() const
	{
		return m_nrOfAttributes;
	}

	void Mesh::free()
	{
		if (m_attributes != nullptr && m_nrOfAttributes != 0)
		{
			for (uint32_t i = 0; i < m_nrOfAttributes; ++i)
			{
				m_attributes[i].free();
			}
			m_nrOfAttributes = 0;
		}
		m_primitiveMode = PrimitiveMode::POINTS;
	}

} // namespace Render
