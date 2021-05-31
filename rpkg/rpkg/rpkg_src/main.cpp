#include "command_line.h"
#include "console.h"
#include "global.h"
#include "file.h"
#include "rpkg.h"
#include "task.h"
#include "generic_function.h"
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        console::display_usage_info();
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
