#pragma once
#include "rpkg.h"
#include <string>
#include <vector>
#include <stdint.h>

#ifdef RPKG_CLI
#define LOG(x) std::cout << x << std::endl
#define LOG_NO_ENDL(x) std::cout << x
#define LOG_AND_EXIT(x) std::cout << x << std::endl; std::exit(0)
#define LOG_AND_RETURN(x) std::cout << x << std::endl; return
#define LOG_AND_EXIT_NOP(x) std::cout << x << std::endl; std::exit(0)
#elif RPKG_DLL
#define LOG(x)
#define LOG_NO_ENDL(x)
#define LOG_AND_EXIT(x) task_status_string = x; task_single_status = TASK_EXITED; task_multiple_status = TASK_EXITED; return
#define LOG_AND_RETURN(x) task_status_string = x; task_single_status = TASK_RETURNED; task_multiple_status = TASK_RETURNED; return
#define LOG_AND_EXIT_NOP(x)
#endif

enum RPKGStatus
{
    READY,
    TASK_EXECUTING,
    TASK_SUCCESSFUL,
    TASK_EXITED,
    TASK_RETURNED,
    RPKG_IMPORTING,
    RPKG_IMPORTED,
    RPKG_ALREADY_IMPORTED,
    SCANNING_DIRECTORY,
    SCANNING_DIRECTORY_DONE,
    ABORT_CURRENT_TASK
};

extern std::vector<rpkg> rpkgs;
extern int percent_progress;
extern std::string response_string;
extern std::vector<char> response_data;
extern std::string timing_string;
extern std::string task_status_string;
extern int task_single_status;
extern int task_multiple_status;
extern int gui_control;
extern std::string localization_string;
extern std::string hashes_with_no_reverse_depends;
extern std::string hash_direct_depends;
extern std::string hash_list_string;
extern std::string patch_deletion_list_string;
extern std::vector<std::string> hashes_depends_map_rpkg_file_paths;
extern std::vector<std::map<uint64_t, uint64_t>> hashes_depends_map;
extern int hash_list_version;
extern bool hash_list_loaded;
extern std::map<uint64_t, uint64_t> hash_list_hash_map;
extern std::vector<std::string> hash_list_hash_file_names;
extern std::vector<std::string> hash_list_hash_value_strings;
extern std::vector<std::string> hash_list_hash_strings;
extern std::string search_imported_hashes_string;
extern std::string search_hash_list_string;
extern std::string rpkg_file_paths_hash_is_in;
extern int pcm_sample_size;
extern int pcm_sample_rate;
extern int pcm_channels;
extern std::string extracted_wem_string;