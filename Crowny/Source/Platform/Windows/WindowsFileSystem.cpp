#include "cwpch.h"

#include "Crowny/Common/FileSystem.h"
#include "Crowny/Application/Application.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#undef GLFW_EXPOSE_NATIVE_WIN32

#include <Windows.h>
#include <commdlg.h>

namespace Crowny
{

	void CALLBACK FileIOCompleted(DWORD dwErrorCode, DWORD dwBytesTransfered, LPOVERLAPPED ol)
	{

	}

	static HANDLE OpenFileForReadingWin32(const std::string& path)
	{
		return CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	}

	static bool ReadFileWin32(HANDLE file, void* buffer, int64_t size)
	{
		OVERLAPPED ol = { 0 };
		return ReadFileEx(file, buffer, size, &ol, FileIOCompleted);
	}

	static int64_t GetFileSizeWin32(HANDLE file)
	{
		LARGE_INTEGER size;
		GetFileSizeEx(file, &size);
		return size.QuadPart;
	}

	bool FileSystem::FileExists(const std::string& path)
	{
		DWORD res = GetFileAttributes(path.c_str()); // is this safe?
		return !(res == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND);
	}

	int64_t FileSystem::GetFileSize(const std::string& path)
	{
		HANDLE file = OpenFileForReadingWin32(path);
		if (file == INVALID_HANDLE_VALUE)
			return -1;
		int64_t size = GetFileSizeWin32(file);
		CloseHandle(file);
		return size;
	}

	byte* FileSystem::ReadFile(const std::string& path)
	{
		HANDLE file = OpenFileForReadingWin32(path);

		int64_t size = GetFileSizeWin32(file);

		byte* buff = new byte[size];
		bool success = ReadFileWin32(file, buff, size);

		CloseHandle(file);

		if (!success)
			delete[] buff;
		return success ? buff : nullptr;
	}

	bool FileSystem::ReadFile(const std::string& path, void* buffer, int64_t size)
	{
		HANDLE file = OpenFileForReadingWin32(path);
		if (file == INVALID_HANDLE_VALUE)
			return false;
		if (size < 0)
		{
			size = GetFileSizeWin32(file);
		}

		bool success = ReadFileWin32(file, buffer, size);
		CloseHandle(file);
		return success;
	}

	std::string FileSystem::ReadTextFile(const std::string& path)
	{
		HANDLE file = OpenFileForReadingWin32(path);
		int64_t size = GetFileSizeWin32(file);
		std::string res(size, 0);
		bool success = ReadFileWin32(file, &res[0], size);
		CloseHandle(file);

		return success ? res : std::string();
	}

	bool FileSystem::WriteFile(const std::string& path, byte* buffer)
	{
		HANDLE file = CreateFile(path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return false;

		int64_t size = GetFileSizeWin32(file);
		DWORD written;
		bool success = ::WriteFile(file, buffer, size, &written, nullptr);
		CloseHandle(file);

		return success;
	}

	bool FileSystem::WriteTextFile(const std::string& path, const std::string& text)
	{
		return WriteFile(path, (byte*)text[0]);
	}

	std::tuple<bool, std::string> FileSystem::OpenFileDialog(const std::string& filter, const std::string& initialDir)
	{
		OPENFILENAME ofn = { 0 };
		TCHAR szFile[260] = { 0 };
		
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == TRUE)
		{
			return std::make_tuple(true, ofn.lpstrFile);
		}

		return std::make_tuple(false, "");
	}

	std::tuple<bool, std::string> FileSystem::SaveFileDialog(const std::string& filter, const std::string& initialDir)
	{
		OPENFILENAME ofn = { 0 };
		TCHAR szFile[260] = { 0 };

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST;

		if (GetSaveFileName(&ofn) == TRUE)
		{
			return std::make_tuple(true, ofn.lpstrFile);
		}

		return std::make_tuple(false, "");
	}

}