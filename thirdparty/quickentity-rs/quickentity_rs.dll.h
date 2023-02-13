#if defined(_WIN32) || defined(_WIN64)
#	ifdef __GNUC__
#		define API_EXPORT __attribute__ ((dllexport))
#		define API_IMPORT __attribute__ ((dllimport))
#	else
#		define API_EXPORT __declspec(dllexport)
#		define API_IMPORT __declspec(dllimport)
#	endif
#       define API_STATIC
#else
#	ifdef __GNUC__
#		define API_EXPORT __attribute__((visibility ("default")))
#		define API_IMPORT __attribute__((visibility ("default")))
#	else
#		define API_EXPORT
#		define API_IMPORT
#	endif
#   define API_STATIC
#endif

extern "C" {
	API_IMPORT char* convert_entity_to_qn(const char* factory_json, const char* factory_meta_json, const char* blueprint_json, const char* blueprint_meta_json);
	API_IMPORT char* free_json_string(const char* json_string);
}