#ifndef JSON_JSONPAIRCONVERTER_HPP_
#define JSON_JSONPAIRCONVERTER_HPP_

#include "JsonPairConverterHelper.hpp"

#include "nlohmann/json.hpp"

#include <functional>
#include <iostream>
#include <optional>
#include <string>

namespace joc
{

class JsonPair
{
public:
    /**
     * @brief Typical constructor for most common T variables
     *        Examples of types supported: booleans, numbers and strings.
     *
     * @param name        the name in the JSON object when calling toJson()
     * @param value       Reference to the T variable from where to read and
     *                    where to write.
     */
    template<typename T>
    JsonPair(const std::string& name, T& value)
        : mIsOptional(false)
        , mAddress(static_cast<void*>(&value))
        , mName(name)
        , mToJsonFunction(templateToJson<T>)
        , mIsJsonValidFunction(templateIsJsonValid<T>)
        , mHasValueFunction([](const void*) { return true; })
        , mFromJsonFunction(templateFromJson<T>)
    {
    }
    template<typename T>
    JsonPair(const std::string& name, std::optional<T>& value)
        : mIsOptional(true)
        , mAddress(static_cast<void*>(&value))
        , mName(name)
        , mToJsonFunction(templateOptionalToJson<T>)
        , mIsJsonValidFunction(templateIsJsonValid<T>)
        , mHasValueFunction(
              [](const void* address) { return static_cast<const std::optional<T>*>(address)->has_value(); })
        , mFromJsonFunction(templateOptionalFromJson<T>)
    {
    }

    bool refreshFromJson(const nlohmann::json& jsonConfig)
    {
        bool isValid = true;
        std::string errorMsg;

        const bool explicitNullOpt = mIsOptional && jsonConfig.contains(mName) && jsonConfig[mName].is_null();
        if (explicitNullOpt)
        {
            // It is valid
        }
        else
        {
            errorMsg = mIsJsonValidFunction(jsonConfig, mName);
            isValid  = errorMsg.empty();
        }

        if (!isValid)
        {
            if (mIsOptional)
            {
                std::cout << "Optional field: '" << mName << "': " << errorMsg << std::endl;
            }
            else
            {
                std::cout << "Mandatory field: '" << mName << "': " << errorMsg << std::endl;
            }
        }
        else
        {
            isValid = mFromJsonFunction(jsonConfig, mName, mAddress);
        }

        return isValid || mIsOptional;
    }

    nlohmann::json::object_t::value_type toJson() const
    {
        return mToJsonFunction(mName, mAddress);
    }

    bool hasValue() const
    {
        return mHasValueFunction(mAddress);
    }

    void* getMutableAddress() const
    {
        return mAddress;
    }

    void setAddress(void* address)
    {
        mAddress = address;
    }

    const std::string& getName() const
    {
        return mName;
    }

private:
    bool mIsOptional;
    void* mAddress;
    std::string mName;
    std::function<nlohmann::json::object_t::value_type(const std::string&, const void*)> mToJsonFunction;
    std::function<std::string(const nlohmann::json&, const std::string&)> mIsJsonValidFunction;
    std::function<bool(const void*)> mHasValueFunction;
    std::function<bool(const nlohmann::json&, const std::string&, void*)> mFromJsonFunction;
};

} // namespace joc

#endif
