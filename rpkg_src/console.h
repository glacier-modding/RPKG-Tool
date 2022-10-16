#pragma once
#include <string>
#include <cstdint>
#include <chrono>

class console
{
public:
	static int update_console(std::string& message, uint64_t indexMax, uint64_t index, std::chrono::time_point<std::chrono::high_resolution_clock> start_time, int stringstream_length);
	static void display_usage_info();
	static void display_licenses();
};

