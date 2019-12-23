#include "helper.h"

#include <iostream>
#include <thread>

#ifdef WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
#endif

namespace
{
	std::filesystem::path GetSystemFontsPath()
	{
#ifdef WINDOWS
		TCHAR path[MAX_PATH];
		GetWindowsDirectory(path, MAX_PATH);
		std::filesystem::path thePath(path);
		thePath = thePath.append("Fonts");
		return thePath;
#else
		// For Linux?
#endif
	}
}


namespace xelous
{
	TimePoint Now()
	{
		return std::chrono::steady_clock::now();
	}

	void Log(const std::string_view& pMessage)
	{
		std::cout << pMessage << "\r\n";
	}

	void SleepFor(std::chrono::milliseconds pMilliseconds)
	{
		std::this_thread::sleep_for(pMilliseconds);
	}

	std::filesystem::path GetFontPath()
	{
		auto path = GetSystemFontsPath();
		path = path.append("Arial.ttf");
		return path;
	}
}