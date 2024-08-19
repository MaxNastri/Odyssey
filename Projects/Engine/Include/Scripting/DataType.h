#pragma once
#include "Core.hpp"
#include "GUID.h"

namespace Odyssey
{

	enum class DataType
	{
		SByte,
		Byte,
		Short,
		UShort,
		Int,
		UInt,
		Long,
		ULong,

		Float,
		Double,

		Vector2,
		Vector3,
		Vector4,

		Bool,
		String,

		Entity,
		Mesh,
		Material,
		Texture2D,
		Scene
	};

	inline uint64_t DataTypeSize(DataType type)
	{
		switch (type)
		{
		case DataType::SByte: return sizeof(int8_t);
		case DataType::Byte: return sizeof(uint8_t);
		case DataType::Short: return sizeof(int16_t);
		case DataType::UShort: return sizeof(uint16_t);
		case DataType::Int: return sizeof(int32_t);
		case DataType::UInt: return sizeof(uint32_t);
		case DataType::Long: return sizeof(int64_t);
		case DataType::ULong: return sizeof(uint64_t);
		case DataType::Float: return sizeof(float);
		case DataType::Double: return sizeof(double);
		case DataType::Vector2: return sizeof(float) * 2;
		case DataType::Vector3: return sizeof(float) * 3;
		case DataType::Vector4: return sizeof(float) * 4;
		case DataType::Bool: return sizeof(Coral::Bool32);
		case DataType::Entity: return sizeof(GUID);
		case DataType::Mesh: return sizeof(GUID);
		case DataType::Material: return sizeof(GUID);
		case DataType::Texture2D: return sizeof(GUID);
		case DataType::Scene: return sizeof(GUID);
		}

		return 0;
	}

	static std::unordered_map<std::string, DataType> g_DataTypeLookup = {
		{ "System.SByte", DataType::SByte },
		{ "System.Byte", DataType::Byte },
		{ "System.Int16", DataType::Short },
		{ "System.UInt16", DataType::UShort },
		{ "System.Int32", DataType::Int },
		{ "System.UInt32", DataType::UInt },
		{ "System.Int64", DataType::Long },
		{ "System.UInt64", DataType::ULong },
		{ "System.Single", DataType::Float },
		{ "System.Double", DataType::Double },
		{ "Odyssey.Vector2", DataType::Vector2 },
		{ "Odyssey.Vector3", DataType::Vector3 },
		{ "Odyssey.Vector4", DataType::Vector4 },
		{ "System.Boolean", DataType::Bool },
		{ "System.String", DataType::String },
		{ "Odyssey.Entity", DataType::Entity },
		{ "Odyssey.Mesh", DataType::Mesh },
		{ "Odyssey.Material", DataType::Material },
		{ "Odyssey.Texture2D", DataType::Texture2D },
		{ "Odyssey.Scene", DataType::Scene },
	};
}