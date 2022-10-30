#include "ResourceConverter.h"
#include "ResourceGenerator.h"
#include "ResourceLibCommon.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#if defined(_MSC_VER)
#	define RESOURCELIB_EXPORT __declspec(dllexport)
#	define RESOURCELIB_IMPORT __declspec(dllimport)
#else
#	define RESOURCELIB_EXPORT __attribute__((visibility("default")))
#	define RESOURCELIB_IMPORT
#endif

#if RESOURCELIB_EXPORTS
#	define RESOURCELIB_API RESOURCELIB_EXPORT
#else
#	define RESOURCELIB_API RESOURCELIB_IMPORT
#endif

#define ZHM_TARGET_FUNC_FINAL(X, Y) HM ## X ## _ ## Y
#define ZHM_TARGET_FUNC_EVAL(X, Y) ZHM_TARGET_FUNC_FINAL(X, Y)
#define ZHM_TARGET_FUNC(FunctionName) ZHM_TARGET_FUNC_EVAL(ZHM_TARGET, FunctionName)

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Get a binary -> json converter for the given resource type (eg. TEMP).
	 * If a converter for this resource type doesn't exist, this function will return [nullptr].
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType);

	/**
	 * Get a json -> binary generator for the given resource type (eg. TEMP).
	 * If a generator for this resource type doesn't exist, this function will return [nullptr].
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType);

	/**
	 * Get a list of resource types that this library supports converting between json and binary forms.
	 * After using the result of this function it must be cleaned up by passing it to the
	 * [HMX_FreeSupportedResourceTypes] function.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)();

	/**
	 * Clean up the supported resource types array. The [ResourceTypesArray] becomes invalid after a
	 * call to this function, and attempting to use it results in undefined behavior.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array);

	/**
	 * Checks if this library supports converting between the json and binary forms of the specified resource types
	 * and returns [true] if it does, or [false] otherwise.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType);

#ifdef __cplusplus
}
#endif

#include "Embind.h"