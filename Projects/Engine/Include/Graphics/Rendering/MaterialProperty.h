#pragma once
#include "RawBuffer.h"

namespace Odyssey
{
	enum class PropertyType
	{
		Unknown = 0,
		Float = 1,
		Float2 = 2,
		Float3 = 3,
		Float4 = 4,
		Bool = 5,
		Int32 = 6,
	};

	struct MaterialProperty
	{
		std::string Name;
		size_t Offset;
		size_t Size;
		PropertyType Type;
	};

	struct MaterialBufferData
	{
	public:
		MaterialBufferData() = default;

		MaterialBufferData(const std::vector<MaterialProperty>& properties, size_t size)
		{
			Set(properties, size);
		}

		void Set(const std::vector<MaterialProperty>& properties, size_t size)
		{
			Size = size;
			Properties = properties;

			// Fill the map
			for (size_t i = 0; i < Properties.size(); i++)
				PropertyMap[Properties[i].Name] = i;

			Buffer.Free();
			Buffer.Allocate(Size);
		}

		void CopyFrom(const MaterialBufferData& other)
		{
			Size = other.Size;
			Properties = other.Properties;
			PropertyMap = other.PropertyMap;

			Buffer.Free();
			RawBuffer::Copy(Buffer, other.Buffer);
		}

		template<typename T>
		T GetValue(const std::string& name)
		{
			if (PropertyMap.contains(name))
			{
				size_t index = PropertyMap[name];
				size_t size = Properties[index].Size;
				size_t offset = Properties[index].Offset;
				return Buffer.Read<T>(offset, size);
			}

			return (T)0;
		}

		void SetValue(const std::string& name, const void* value)
		{
			if (PropertyMap.contains(name))
			{
				size_t index = PropertyMap[name];
				size_t size = Properties[index].Size;
				size_t offset = Properties[index].Offset;
				return Buffer.Write(value, size, offset);
			}
		}

	public:
		std::unordered_map<std::string, size_t> PropertyMap;
		std::vector<MaterialProperty> Properties;
		size_t Size;
		RawBuffer Buffer;
	};
}