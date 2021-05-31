#include "command_line.h"
#include "file.h"
#include "global.h"
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <sstream>

bool command_line::all_args_are_paths(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (!file::path_exists(std::string(argv[i])))
        {
            return false;
        }
    }

    return true;
}

std::vector<std::vector<std::string>> command_line::parse(int argc, char* argv[])
{
    std::vector<std::vector<std::string>> command_line_args;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                std::vector<std::string> temp_command_line_args;

                temp_command_line_args.push_back(argv[i]);

                if (argc > (i + 1))
                {
                    if (argv[i + 1][0] != '-' && std::string(argv[i + 1]).length() > 0)
                    {
                        temp_command_line_args.push_back(argv[i + 1]);
                    }
                    else
                    {
                        LOG_AND_EXIT_NOP("Error: Invalid command line input.");
                    }
                }
                else
                {
                    LOG_AND_EXIT_NOP("Error: Invalid command line input.");
                }

                command_line_args.push_back(temp_command_line_args);
            }
        }
    }

    return command_line_args;
}

std::vector<std::string> command_line::get_dragged_and_dropped_files(int argc, char* argv[])
{
    std::vector<std::string> dragged_and_dropped_files;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            dragged_and_dropped_files.push_back(argv[i]);
        }
    }

    return dragged_and_dropped_files;
}