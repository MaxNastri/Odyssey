#include "FileDialogs.h"
#include "glfw3.h"
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // !GLFW_EXPOSE_NATIVE_WIN32
#include "glfw3native.h"
#include <commdlg.h>
#include "Renderer.h"
#include "VulkanWindow.h"

namespace Odyssey
{
	std::string GetFilter(std::string_view fileType, const Path& extension)
	{
		//(*.ext) FileType\0*.ext\0"
		std::string filter;
		filter.append(std::format("(*{})", extension.string()));
		filter.append(std::format(" {}", fileType));
		filter.push_back('\0');
		filter.append(std::format("*{}", extension.string()));
		filter.push_back('\0');
		return filter;
	}

	Path FileDialogs::OpenFile(std::string_view fileType, const Path& extension)
	{
		std::string filter = GetFilter(fileType, extension);
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		void* window = Renderer::GetWindow()->GetNativeWindow();
		ofn.hwndOwner =  glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn))
		{
			Path path = ofn.lpstrFile;

			if (!path.has_extension() && !extension.empty())
				path = path.replace_extension(extension);

			return path;
		}

		return std::string();
	}

	Path FileDialogs::SaveFile(std::string_view fileType, const Path& extension)
	{
		std::string filter = GetFilter(fileType, extension);

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		void* window = Renderer::GetWindow()->GetNativeWindow();
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn))
		{
			Path path = ofn.lpstrFile;

			if (!path.has_extension() && !extension.empty())
				path = path.replace_extension(extension);

			return path;
		}

		return std::string();
	}
}