#pragma once

namespace Odyssey
{
	class VulkanTexture2D
	{
	public:
		VulkanTexture2D(std::string_view filename);

	private:
		void LoadFromFile(std::string_view filename);

	};
}