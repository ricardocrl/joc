#include "JsonObjectConverter.hpp"

namespace joc
{

void to_json(nlohmann::json& j, const JsonObject& joc)
{
    j = joc.toJson();
}

void from_json(const nlohmann::json& j, JsonObject& joc)
{
    joc.refreshFromJson(j);
}

#define LOCK_SCOPE() \
    auto lk = (mDataMutex == nullptr) ? std::unique_lock<std::mutex>() : std::unique_lock<std::mutex>(*mDataMutex);

JsonObject::JsonObject(const std::vector<JsonPair>& pairs, std::mutex* dataMutex)
    : mPairs(pairs)
    , mDataMutex(dataMutex)
{
}

JsonObject::JsonObject(const JsonObject& other)
    : mPairs(other.mPairs)
// we cannot reuse the mutex from another object, don't assign mDataMutex
{
    updateAddresses(other);
}

JsonObject& JsonObject::operator=(const JsonObject& /*other*/)
{
    // Skipping copy of mPairs as they already point to the members of the child classes of
    // this object (created with other constructors)
    return *this;
}

void JsonObject::setDataMutex(std::mutex* dataMutex)
{
    mDataMutex = dataMutex;
}

void JsonObject::updateAddresses(const JsonObject& origin)
{
    auto ptrInt      = reinterpret_cast<std::uintptr_t>(static_cast<JsonObject*>(this));
    auto otherPtrInt = reinterpret_cast<std::uintptr_t>(static_cast<const JsonObject*>(&origin));
    std::uintptr_t diff;
    if (ptrInt > otherPtrInt)
    {
        diff = ptrInt - otherPtrInt;
    }
    else
    {
        diff = otherPtrInt - ptrInt;
    }

    for (size_t i = 0; i < mPairs.size(); ++i)
    {
        if (ptrInt > otherPtrInt)
        {
            mPairs[i].setAddress(
                reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(origin.mPairs[i].getMutableAddress()) + diff));
        }
        else
        {
            mPairs[i].setAddress(
                reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(origin.mPairs[i].getMutableAddress()) - diff));
        }
    }
}

nlohmann::json JsonObject::toJson() const
{
    auto jsonConfig = nlohmann::json::object();

    LOCK_SCOPE();
    for (const auto& p : mPairs)
    {
        if (p.hasValue())
        {
            jsonConfig += p.toJson();
        }
    }
    return jsonConfig;
}

bool JsonObject::refreshFromJson(const nlohmann::json& jsonConfig)
{
    if (jsonConfig.empty())
    {
        return false;
    }
    auto success = true;

    LOCK_SCOPE();
    for (auto& p : mPairs)
    {
        if (!p.refreshFromJson(jsonConfig))
        {
            success = false;
        }
    }
    return success;
}
} // namespace json
