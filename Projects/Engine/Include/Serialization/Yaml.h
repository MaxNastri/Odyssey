#pragma once
#include "ryml.hpp"
#include "glm.h"
#include <fstream>

namespace glm
{
	inline size_t to_chars(ryml::substr buff, glm::vec2 vec2)
	{
		return ryml::format(buff, "x: {} y: {}", vec2.x, vec2.y);
	}

	inline bool from_chars(ryml::csubstr buff, glm::vec2* vec2)
	{
		size_t ret = ryml::unformat(buff, "x: {} y: {}", vec2->x, vec2->y);
		return ret != ryml::yml::npos;
	}

	inline size_t to_chars(ryml::substr buff, glm::vec3 vec3)
	{
		return ryml::format(buff, "x: {} y: {} z: {}", vec3.x, vec3.y, vec3.z);
	}

	inline bool from_chars(ryml::csubstr buff, glm::vec3* vec3)
	{
		size_t ret = ryml::unformat(buff, "x: {} y: {} z: {}", vec3->x, vec3->y, vec3->z);
		return ret != ryml::yml::npos;
	}
	inline size_t to_chars(ryml::substr buff, glm::vec4 vec4)
	{
		return ryml::format(buff, "x: {} y: {} z: {} w: {}", vec4.x, vec4.y, vec4.z, vec4.w);
	}

	inline bool from_chars(ryml::csubstr buff, glm::vec4* vec4)
	{
		size_t ret = ryml::unformat(buff, "x: {} y: {} z: {} w: {}", vec4->x, vec4->y, vec4->z, vec4->w);
		return ret != ryml::yml::npos;
	}

	inline size_t to_chars(ryml::substr buff, glm::quat quat)
	{
		return ryml::format(buff, "x: {} y: {} z: {} w: {}", quat.x, quat.y, quat.z, quat.w);
	}

	inline bool from_chars(ryml::csubstr buff, glm::quat* quat)
	{
		size_t ret = ryml::unformat(buff, "x: {} y: {} z: {} w: {}", quat->x, quat->y, quat->z, quat->w);
		return ret != ryml::yml::npos;
	}

	inline size_t to_chars(ryml::substr buff, glm::mat4 mat)
	{
		return ryml::format(buff, "00: {} 01: {} 02: {} 03: {} 10: {} 11: {} 12: {} 13: {} 20: {} 21: {} 22: {} 23: {} 30: {} 31: {} 32: {} 33: {}",
			mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	}

	inline bool from_chars(ryml::csubstr buff, glm::mat4* mat)
	{
		glm::mat4x4 matBuffer;

		size_t ret = ryml::unformat(buff, "00: {} 01: {} 02: {} 03: {} 10: {} 11: {} 12: {} 13: {} 20: {} 21: {} 22: {} 23: {} 30: {} 31: {} 32: {} 33: {}", 
			matBuffer[0][0], matBuffer[0][1], matBuffer[0][2], matBuffer[0][3],
			matBuffer[1][0], matBuffer[1][1], matBuffer[1][2], matBuffer[1][3],
			matBuffer[2][0], matBuffer[2][1], matBuffer[2][2], matBuffer[2][3],
			matBuffer[3][0], matBuffer[3][1], matBuffer[3][2], matBuffer[3][3]);
		*mat = matBuffer;
		return ret != ryml::yml::npos;
	}
}
namespace Odyssey
{
	static void YamlSample()
	{
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["Name"] = "Scene.yaml";

		ryml::NodeRef gameObjectsNodes = root["GameObjects"];
		gameObjectsNodes |= ryml::SEQ;

		for (int i = 0; i < 10; ++i)
		{
			ryml::NodeRef gameObjectNode = gameObjectsNodes.append_child();
			gameObjectNode |= ryml::MAP;
			gameObjectNode["Name"] << std::string("GameObject " + std::to_string(i));
			gameObjectNode["Active"] << true;

			ryml::NodeRef componentsNode = gameObjectNode["Components"];
			componentsNode |= ryml::SEQ;

			for (int c = 0; c < 2; ++c)
			{
				glm::vec2 vec2(1, 2);
				glm::vec3 vec3(33,222,1111);
				glm::vec4 vec4(1, 2, 3, 4);
				glm::quat quat(4, 3, 2, 1);
				glm::mat4x4 mat(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
				ryml::NodeRef componentNode = componentsNode.append_child();
				componentNode |= ryml::MAP;
				componentNode["Type"] << "UserScript";
				componentNode["Name"] << std::string("Component " + std::to_string(c));
				componentNode["fltValue"] << 420.69f;
				componentNode["pos"] << vec2;
				componentNode["rot"] << vec3;
				componentNode["scale"] << vec4;
				componentNode["rotation"] << quat;
				componentNode["world"] << mat;
			}
		}

		FILE* file = fopen("scene.yaml", "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file);
		fclose(file);
	}
}