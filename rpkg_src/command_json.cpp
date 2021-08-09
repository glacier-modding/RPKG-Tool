#include "generic_function.h"
#include "util.h"
#include "file.h"
#include "task.h"
#include "global.h"
#include "thirdparty/json/json.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using json = nlohmann::ordered_json;

void generic_function::command_json(std::string& json_file_path)
{
    std::ifstream command_json_file(json_file_path, std::ifstream::binary);

    if (!command_json_file.good())
    {
        LOG_AND_EXIT("Error: JSON file " + json_file_path + " could not be read.");
    }

    json command_json;

    command_json_file >> command_json;

    command_json_file.close();

    LOG("Parsing input " << json_file_path << " command JSON");

    LOG("    Commands found: " << command_json.size());

    for (const auto& it : command_json.items())
    {
        std::string command = "";
        std::string input_path = "";
        std::string filter = "";
        std::string search = "";
        std::string search_type = "";
        std::string output_path = "";

        if (it.value().contains("Command"))
        {
            if (util::to_upper_case(it.value()["Command"]) == "COMPUTE_IOI_HASH")
            {
                std::string command = "-compute_ioi_hash";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("StringToHash"))
                {
                    filter = std::string(it.value()["StringToHash"]);

                    if (filter == "")
                    {
                        LOG_AND_EXIT("Error: No input string to hash specified in the input JSON.");
                    }

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: compute_ioi_hash");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  StringToHash: " << filter);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command compute_ioi_hash requires a \"StringToHash\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "DECRYPT_PACKAGEDEFINITION_THUMBS")
            {
                std::string command = "-decrypt_packagedefinition_thumbs";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("FileToDecrypt"))
                {
                    input_path = std::string(it.value()["FileToDecrypt"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: decrypt_packagedefinition_thumbs");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  FileToDecrypt: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command decrypt_packagedefinition_thumbs requires a \"FileToDecrypt\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "ENCRYPT_PACKAGEDEFINITION_THUMBS")
            {
                std::string command = "-encrypt_packagedefinition_thumbs";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("FileToEncrypt"))
                {
                    input_path = std::string(it.value()["FileToEncrypt"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: encrypt_packagedefinition_thumbs");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  FileToEncrypt: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command encrypt_packagedefinition_thumbs requires a \"FileToEncrypt\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_FROM_RPKG")
            {
                std::string command = "-extract_from_rpkg";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }

                if (it.value().contains("InputFilePath"))
                {
                    input_path = std::string(it.value()["InputFilePath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_from_rpkg");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFilePath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_from_rpkg requires a \"InputFilePath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_ORES_FROM")
            {
                std::string command = "-extract_ores_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_ores_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_ores_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_WWEM_TO_OGG_FROM")
            {
                std::string command = "-extract_wwem_to_ogg_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_wwem_to_ogg_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_wwem_to_ogg_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_WWES_TO_OGG_FROM")
            {
                std::string command = "-extract_wwes_to_ogg_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_wwes_to_ogg_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_wwes_to_ogg_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_WWEV_TO_OGG_FROM")
            {
                std::string command = "-extract_wwev_to_ogg_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_wwev_to_ogg_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_wwev_to_ogg_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_DLGE_TO_JSON_FROM")
            {
                std::string command = "-extract_dlge_to_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_dlge_to_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_dlge_to_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_LOCR_TO_JSON_FROM")
            {
                std::string command = "-extract_locr_to_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_locr_to_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_locr_to_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXTRACT_RTLV_TO_JSON_FROM")
            {
                std::string command = "-extract_rtlv_to_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: extract_rtlv_to_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command extract_rtlv_to_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "GENERATE_RPKG_FROM")
            {
                std::string command = "-generate_rpkg_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: generate_rpkg_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command generate_rpkg_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "HASH_DEPENDS")
            {
                std::string command = "-hash_depends";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command hash_depends requires a \"Filter\" value in the input JSON.");
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: hash_depends");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command hash_depends requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "HASH_PROBE")
            {
                std::string command = "-hash_probe";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("Filter"))
                {
                    filter = std::string(it.value()["Filter"]);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command hash_probe requires a \"Filter\" value in the input JSON.");
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: hash_probe");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    if (filter.length() > 0)
                    {
                        LOG("  Filter: " << filter);
                    }
                    else
                    {
                        LOG("  Filter: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command hash_probe requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "REBUILD_DLGE_FROM_JSON_FROM")
            {
                std::string command = "-rebuild_dlge_from_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: rebuild_dlge_from_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command rebuild_dlge_from_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "REBUILD_LOCR_FROM_JSON_FROM")
            {
                std::string command = "-rebuild_locr_from_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: rebuild_locr_from_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command rebuild_locr_from_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "REBUILD_RTLV_FROM_JSON_FROM")
            {
                std::string command = "-rebuild_rtlv_from_json_from";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: rebuild_rtlv_from_json_from");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command rebuild_rtlv_from_json_from requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "REBUILD_WWEV_IN")
            {
                std::string command = "-rebuild_wwev_in";

                if (it.value().contains("OutputPath"))
                {
                    output_path = std::string(it.value()["OutputPath"]);

                    file::create_directories(output_path);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    input_path = std::string(it.value()["InputFolderPath"]);

                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: rebuild_wwev_in");

                    if (output_path != "")
                    {
                        LOG("  OutputPath: " << output_path);
                    }
                    else
                    {
                        LOG("  OutputPath: None");
                    }

                    LOG("  InputFolderPath: " << input_path);

                    task::execute(command, input_path, filter, search, search_type, output_path);
                }
                else
                {
                    LOG_AND_EXIT("Error: Command rebuild_wwev_in requires a \"InputFolderPath\" value in the input JSON.");
                }
            }
            else if (util::to_upper_case(it.value()["Command"]) == "EXECUTE_SYSTEM_COMMAND")
            {
                std::string command = "-execute_system_command";

                if (it.value().contains("CommandLine"))
                {
                    LOG("Executing JSON command number " << it.key());
                    LOG("  Command: execute_system_command");
                    LOG("  CommandLine: " << it.value()["CommandLine"]);

                    std::system(std::string(it.value()["CommandLine"]).c_str());
                }
                else
                {
                    LOG_AND_EXIT("Error: Command execute_system_command requires a \"CommandLine\" value in the input JSON.");
                }
            }

            LOG_NO_ENDL(std::endl);
        }
    }
}
