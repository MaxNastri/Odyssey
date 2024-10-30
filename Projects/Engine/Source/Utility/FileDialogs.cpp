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
	Path FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		void* window = Renderer::GetWindow()->GetNativeWindow();
		ofn.hwndOwner =  glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}

		return std::string();
	}

	Path FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		void* window = Renderer::GetWindow()->GetNativeWindow();
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}

		return std::string();
	}
}