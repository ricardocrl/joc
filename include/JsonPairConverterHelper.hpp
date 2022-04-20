#ifndef JSON_JSONPAIRCONVERTERHELPER_HPP_
#define JSON_JSONPAIRCONVERTERHELPER_HPP_

#include "nlohmann/json.hpp"

#include <chrono>
#include <string>
#include <type_traits>

namespace std
{
namespace chrono
{

template<typename Rep, typename Per>
void to_json(nlohmann::json& j, const duration<Rep, Per>& cpp)
{
    j = cpp.count();
}

template<typename _Rep, typename _Per>
void from_json(const nlohmann::json& j, duration<_Rep, _Per>& cpp)
{
    cpp = std::chrono::duration<_Rep, _Per>(j);
}

template<typename _Clock, typename _Dur>
void to_json(nlohmann::json& j, const time_point<_Clock, _Dur>& cpp)
{
    to_json(j, duration_cast<seconds>(cpp.time_since_epoch()));
}

template<typename _Clock, typename _Dur>
void from_json(const nlohmann::json& j, time_point<_Clock, _Dur>& cpp)
{
    cpp = std::chrono::time_point<_Clock, _Dur>(seconds(j));
}

} // namespace chrono
} // namespace std

namespace joc
{

using SystemClock     = std::chrono::system_clock;
using SteadyClock     = std::chrono::steady_clock;
using SystemTimePoint = std::chrono::time_point<SystemClock>;
using SteadyTimePoint = std::chrono::time_point<SteadyClock>;

template<typename T>
nlohmann::json::object_t::value_type templateToJson(const std::string& name, const void* value)
{
    return {name, *static_cast<const T*>(value)};
}

template<typename T>
nlohmann::json::object_t::value_type templateOptionalToJson(const std::string& name, const void* optional)
{
    const std::optional<T>& optionalType = *static_cast<const std::optional<T>*>(optional);
    return templateToJson<T>(name, &optionalType.value());
}

template<typename T>
nlohmann::json getDummyJsonFromCppVar()
{
    return nlohmann::json(T());
}

template<>
inline nlohmann::json getDummyJsonFromCppVar<std::chrono::nanoseconds>()
{
    return nlohmann::json(int64_t());
}
template<>
inline nlohmann::json getDummyJsonFromCppVar<std::chrono::microseconds>()
{
    return nlohmann::json(int64_t());
}
template<>
inline nlohmann::json getDummyJsonFromCppVar<std::chrono::milliseconds>()
{
    return nlohmann::json(int64_t());
}
template<>
inline nlohmann::json getDummyJsonFromCppVar<std::chrono::seconds>()
{
    return nlohmann::json(int64_t());
}
template<>
inline nlohmann::json getDummyJsonFromCppVar<joc::SystemTimePoint>()
{
    return nlohmann::json(int64_t());
}
template<>
inline nlohmann::json getDummyJsonFromCppVar<joc::SteadyTimePoint>()
{
    return nlohmann::json(int64_t());
}

template<typename T>
std::string templateIsJsonValid(const nlohmann::json& jsonConfig, const std::string& name)
{
    if (!jsonConfig.contains(name))
    {
        return "is not provided";
    }
    if (jsonConfig[name].type_name() != getDummyJsonFromCppVar<T>().type_name())
    {
        std::string msg = "is of invalid type, expected: ";
        msg += getDummyJsonFromCppVar<T>().type_name();
        msg += " but is: ";
        msg += jsonConfig[name].type_name();
        return msg;
    }
    return std::string();
}

template<>
inline std::string templateIsJsonValid<nlohmann::json>(const nlohmann::json& jsonConfig, const std::string& name)
{
    if (!jsonConfig.contains(name))
    {
        return "is not provided";
    }
    return std::string();
}

template<typename T>
bool templateFromJson(const nlohmann::json& jsonConfig, const std::string& name, void* value)
{
    jsonConfig[name].get_to<T>(*static_cast<T*>(value));
    return true;
}

template<>
inline bool
templateFromJson<nlohmann::json>(const nlohmann::json& jsonConfig, const std::string& name, void* value)
{
    *static_cast<nlohmann::json*>(value) = jsonConfig[name];
    return true;
}

template<typename T>
bool templateOptionalFromJson(const nlohmann::json& jsonConfig, const std::string& name, void* optional)
{
    std::optional<T>& optionalType = *static_cast<std::optional<T>*>(optional);
    optionalType                   = T();
    if (jsonConfig[name].is_null())
    {
        optionalType = std::nullopt;
        return true;
    }
    return templateFromJson<T>(jsonConfig, name, &optionalType.value());
}
} // namespace json

#endif
