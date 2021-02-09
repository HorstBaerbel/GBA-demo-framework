#pragma once

#include "attributes.h"

namespace Render
{

/// @brief Can store either source / constant or generated / transformed interleaved vertex data
/// Initialize with either const source data (data != nullptr, count > 0),
/// or allocate memory in EWRAM with (data == nullptr, count > 0).
struct VertexData
{

	struct DataInfo
	{
		uint8_t *data;		// raw pointer to data
		uint16_t count;		// number of elements of attribute type
		AttributeType type; // type of attribute
	};

	/// @brief Will store constant or allocate data for attributes.
	/// Pass data != nullptr and count > 0 to store pointer to data,
	/// or pass a data == nullptr and count > 0 to allocate space for data in EWRAM.
	void allocate(std::initializer_list<DataInfo> l);
	/// @brief Get pointer to data.
	uint8_t *attribute(AttributeType type) const;
	/// @brief Get pointer to data.
	uint16_t count(AttributeType type) const;
	/// @brief Free buffer data again.
	void free();

private:
	struct InternalDataInfo
	{
		uint8_t *data;
		uint16_t count;
		AttributeType type;
		uint8_t allocated;
	};

	static constexpr uint32_t MAX_ATTRIBUTES = 5;
	InternalDataInfo m_data[MAX_ATTRIBUTES];
};

} // namespace Render
