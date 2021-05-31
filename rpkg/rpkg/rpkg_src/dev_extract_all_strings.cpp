#include "rpkg_function.h"
#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void dev_function::dev_extract_all_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
	input_path = file::parse_input_folder_path(input_path);

	rpkg_function::import_rpkg_files_in_folder(input_path);

	dev_function::dev_extract_wwem_strings(input_path, filter, output_path);

	dev_function::dev_extract_wwes_strings(input_path, filter, output_path);

	dev_function::dev_extract_wwev_strings(input_path, filter, output_path);

	dev_function::dev_extract_materials_textures_strings(input_path, filter, output_path);
}