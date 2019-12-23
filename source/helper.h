#pragma once

#include <sstream>
#include <string_view>
#include <chrono>
#include <filesystem>

namespace xelous
{
	using TimePoint = std::chrono::steady_clock::time_point;

	TimePoint Now();

	void Log(const std::string_view& pMessage);

	void SleepFor(std::chrono::milliseconds pMilliseconds);

	std::filesystem::path GetFontPath();
}