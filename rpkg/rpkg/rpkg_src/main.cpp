#include "command_line.h"
#include "console.h"
#include "global.h"
#include "file.h"
#include "rpkg.h"
#include "task.h"
#include "generic_function.h"
#include <string>
#include <vector>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        console::display_usage_info();
    }
    else if (argc == 2)
    {
        if (std::string(argv[1]) == "-i")
        {
            interative_mode = true;

            std::string input;

            std::cout << "Type q or quit to exit rpkg-cli, and type h or help to display usage info." << std::endl << std::endl;

            std::cout << "RPKG> ";

            while (std::getline(std::cin, input))
            {
                if (input == "q" || input == "quit")
                {
                    std::exit(0);
                }
                else if (input == "h" || input == "help")
                {
                    console::display_usage_info();
                }

                std::vector<std::string> args;

                size_t pos1 = 0;
                size_t pos2 = input.find(" ");

                bool no_errors = true;

                while (pos2 != std::string::npos && no_errors)
                {
                    args.push_back(input.substr(pos1, pos2));

                    pos1 += pos2 + 1;

                    if (pos1 < input.length())
                    {
                        if (input[pos1] == '\'')
                        {
                            size_t pos3 = input.substr(pos1 + 1).find("\'");

                            if (pos3 != std::string::npos)
                            {
                                args.push_back(input.substr(pos1 + 1, pos3));

                                pos1 += pos3 + 2;
                            }
                        }
                        else if (input[pos1] == '\"')
                        {
                            size_t pos3 = input.substr(pos1 + 1).find("\"");

                            if (pos3 != std::string::npos)
                            {
                                args.push_back(input.substr(pos1 + 1, pos3));

                                pos1 += pos3 + 2;
                            }
                        }
                    }

                    pos2 = input.substr(pos1).find(" ");
                }

                if (pos1 < input.length())
                {
                    args.push_back(input.substr(pos1));
                }

                if (args.size() > 0)
                {
                    std::cout << "Running command: " << input << std::endl << std::endl;

                    std::vector<std::vector<std::string>> commands = command_line::parse(args);

                    task::process_and_execute_command_line_args(commands);
                }
                else
                {
                    std::cout << "Invalid command: " << input << std::endl << std::endl;
                }

                std::cout << std::endl << "RPKG> ";
            }
        }
        else
        {
            console::display_usage_info();
        }
    }
    else
    {
        if (command_line::all_args_are_paths(argc, argv))
        {
            std::vector<std::string> dragged_and_dropped_files = command_line::get_dragged_and_dropped_files(argc, argv);

            task::process_and_execute_files_draged_and_dropped(dragged_and_dropped_files);
        }
        else
        {
            std::vector<std::vector<std::string>> commands = command_line::parse(argc, argv);

            task::process_and_execute_command_line_args(commands);
        }
    }
}
