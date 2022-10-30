#pragma once

#if __EMSCRIPTEN__

#include <emscripten/bind.h>

#include <vector>
#include <string>

using namespace emscripten;

#define ZHM_MODULE_NAME_FOR_TARGET(TARGET) HM ## _ ## TARGET
#define ZHM_MODULE_NAME ZHM_MODULE_NAME_FOR_TARGET(ZHM_TARGET)

std::vector<std::string> GetSupportedResourceTypes()
{
	auto* s_Resources = ZHM_TARGET_FUNC(GetSupportedResourceTypes)();

	std::vector<std::string> s_TypesVector;

	for (size_t i = 0; i < s_Resources->TypeCount; ++i)
		s_TypesVector.push_back(s_Resources->Types[i]);

	ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(s_Resources);

	return s_TypesVector;
}

bool IsResourceTypeSupported(const std::string& p_ResourceType)
{
	return ZHM_TARGET_FUNC(IsResourceTypeSupported)(p_ResourceType.c_str());
}

val ConvertResourceToJson(const std::string& p_ResourceType, const std::string& p_ResourceData)
{
	auto* s_Converter = ZHM_TARGET_FUNC(GetConverterForResource)(p_ResourceType.c_str());

	if (s_Converter == nullptr)
		return val::null();

	auto* s_Result = s_Converter->FromMemoryToJsonString(p_ResourceData.data(), p_ResourceData.size());

	if (s_Result == nullptr)
		return val::null();

	std::string s_JsonData(s_Result->JsonData, s_Result->StrSize);

	s_Converter->FreeJsonString(s_Result);

	return val(s_JsonData);
}

val GenerateResourceFromJson(const std::string& p_ResourceType, const std::string& p_ResourceData)
{
	auto* s_Generator = ZHM_TARGET_FUNC(GetGeneratorForResource)(p_ResourceType.c_str());

	if (s_Generator == nullptr)
		return val::null();

	auto* s_Result = s_Generator->FromJsonStringToResourceMem(p_ResourceData.data(), p_ResourceData.size(), false);

	if (s_Result == nullptr)
		return val::null();

	emscripten::val s_View(emscripten::typed_memory_view(s_Result->DataSize, reinterpret_cast<const uint8_t*>(s_Result->ResourceData)));

	auto s_JSResult = emscripten::val::global("Uint8Array").new_(s_Result->DataSize);
	s_JSResult.call<void>("set", s_View);

	s_Generator->FreeResourceMem(s_Result);

	return s_JSResult;
}

EMSCRIPTEN_BINDINGS(ZHM_MODULE_NAME)
{
	register_vector<std::string>("vector<string>");
	
	function("GetSupportedResourceTypes", &GetSupportedResourceTypes);
	function("IsResourceTypeSupported", &IsResourceTypeSupported);
	function("ConvertResourceToJson", &ConvertResourceToJson);
	function("GenerateResourceFromJson", &GenerateResourceFromJson);
}

#endif