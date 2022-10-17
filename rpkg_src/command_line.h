#pragma once
#include <string>
#include <vector>

class command_line
{
public:
	static bool all_args_are_paths(int argc, char* argv[]);
	static std::vector<std::vector<std::string>> parse(int argc, char* argv[]);
	static std::vector<std::vector<std::string>> parse(std::vector<std::string> args);
	static std::vector<std::string> get_dragged_and_dropped_files(int argc, char* argv[]);
};
