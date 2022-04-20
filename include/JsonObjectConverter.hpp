#ifndef JSONOBJECTCONVERTER_HPP_
#define JSONOBJECTCONVERTER_HPP_

#include "JsonPairConverter.hpp"

#include "nlohmann/json.hpp"

#include <mutex>
#include <vector>

namespace joc
{
/**
 * @brief This class can be used as a base class for JSON-based data containers.
 *        It provides the means for:
 *
 *          1. Definition of the JSON "schema":
 *             - Keys and values are passed to the constructor as a JsonPair vector.
 *             - The memory location of the member variables of the derived class
 *               (user class) are stored to allow joclib to have read/write access.
 *             NOTE: JsonObject does not provide means for schema validation,
 *                   the derived class is responsible for this.
 *
 *          2. Conversion between the derived class member variables
 *             and JSON format that represent them.
 *
 *          3. Protecting toJson and refreshFromJson operations by locking a mutex
 *             passed (optionally) to the constructor or set by setDataMutex method.
 *
 *             NOTE: The derived class is responsible for NOT locking the same mutex
 *             while calling toJson or refreshFromJson from the same thread.
 *
 *        Example: see joc_sample.cpp
 */
class JsonObject
{
public:
    JsonObject(const std::vector<JsonPair>& pairs = {}, std::mutex* dataMutex = nullptr);
    virtual ~JsonObject() = default;

    /**
     * See Cellcyte class examples that use JsonObject as
     * a reference.
     */
    JsonObject(const JsonObject&);
    JsonObject& operator=(const JsonObject&);

    JsonObject(JsonObject&&) = delete;
    JsonObject& operator=(JsonObject&&) = delete;

    /**
     * @brief Outputs a JSON object from its vector of JsonPairs
     *        For each JsonPairs, a json::object_t::value_type is generated,
     *        which is not a json object in itself, but an entry in a json object.
     * @return JSON object from its vector of JsonPairs
     */
    nlohmann::json toJson() const;

    /**
     * @brief Converts a given JSON to the internal values.
     *        Each JsonPair stores a pointer to a variable that
     *        represents the value in the (key, value) pair. This function
     *        hands over the given JSON to each JsonPair and they
     *        will look for their key in the JSON and if it is valid will
     *        populate the value from the JSON and assign it to the
     *        variable bound to it.
     *
     *        The checks for a successful operation are:
     *        - The key was found in the JSON
     *        - The type is correct
     *
     *        If one of these fails AND the C++ variable is not an std::optional,
     *        the operation will fail, returning false.
     *
     * @param jsonConfig the JSON from which to import the values
     * @return true if all pairs succeeded populating their values from JSON
     */
    bool refreshFromJson(const nlohmann::json& jsonConfig);

    /**
     * @brief Set data mutex to protect read/writes of data. When a mutex is
     *        needed to protect data, it can be set in the constructor,
     *        except for the copy constructor. In such case, this setter is
     *        recommended.
     */
    void setDataMutex(std::mutex* dataMutex);

private:
    std::vector<JsonPair> mPairs;

    std::mutex* mDataMutex{nullptr};

    void updateAddresses(const JsonObject& origin);
};

void to_json(nlohmann::json& j, const JsonObject& joc);
void from_json(const nlohmann::json& j, JsonObject& joc);

} // namespace joc

#endif
