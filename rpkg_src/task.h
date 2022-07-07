#pragma once
#include "global.h"
#include <string>
#include <vector>

class task
{
public:
	static void execute(std::string& command, std::string& input_path, std::string& filter, std::string& search, std::string& search_type, std::string& output_path);
	static void process_and_execute_files_dragged_and_dropped(std::vector<std::string>& dragged_and_dropped_files);
	static void process_and_execute_command_line_args(std::vector<std::vector<std::string>>& commands);
};

