#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	struct ResourceMem;
	
	struct ResourceGenerator
	{
		/**
		 * Generate a new binary resource from the json file found at [p_JsonFilePath] and store it
		 * in the [p_ResourceFilePath] file on the filesystem.
		 *
		 * The [p_Simple] parameter specifies whether the json input is in its simplified for
		 * or not.
		 *
		 * Returns [true] on success, or [false] otherwise.
		 */
		bool (*FromJsonFileToResourceFile)(const char* p_JsonFilePath, const char* p_ResourceFilePath, bool p_Simple);

		/**
		 * Generate a new binary resource from the specified json string and store it in the
		 * [p_ResourceFilePath] file on the filesystem. The [p_JsonStr] must point to the json
		 * string data and the [p_JsonStrLength] must be the length of the data, excluding any
		 * null terminators.
		 *
		 * The [p_Simple] parameter specifies whether the json input is in its simplified for
		 * or not.
		 *
		 * Returns [true] on success, or [false] otherwise.
		 */
		bool (*FromJsonStringToResourceFile)(const char* p_JsonStr, size_t p_JsonStrLength, const char* p_ResourceFilePath, bool p_Simple);

		/**
		 * Generate a new binary resource from the json file found at [p_JsonFilePath] and store it
		 * in memory as a [ResourceMem]. After using it, the [ResourceMem] must be cleaned up
		 * by passing it to the [FreeResourceMem] function.
		 *
		 * The [p_Simple] parameter specifies whether the json input is in its simplified for
		 * or not.
		 *
		 * If the operation fails, this function will return [nullptr].
		 */
		ResourceMem* (*FromJsonFileToResourceMem)(const char* p_JsonFilePath, bool p_Simple);

		/**
		 * Generate a new binary resource from the specified json string and store it in memory as
		 * a [ResourceMem]. The [p_JsonStr] must point to the json string data and the
		 * [p_JsonStrLength] must be the length of the data, excluding any null terminators. After
		 * using it, the [ResourceMem] must be cleaned up by passing it to the [FreeResourceMem]
		 * function.
		 *
		 * The [p_Simple] parameter specifies whether the json input is in its simplified for
		 * or not.
		 *
		 * If the operation fails, this function will return [nullptr].
		 */
		ResourceMem* (*FromJsonStringToResourceMem)(const char* p_JsonStr, size_t p_JsonStrLength, bool p_Simple);

		/**
		 * Clean up an in-memory binary resource. The [ResourceMem] becomes invalid after a call
		 * to this function, and attempting to use it results in undefined behavior.
		 */
		void (*FreeResourceMem)(ResourceMem* p_ResourceMem);
	};

#ifdef __cplusplus
}
#endif
