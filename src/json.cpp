#include "nanoros/json.h"


using namespace ssr::nanoros;


class JSONObjectImpl : public JSONObject {

public:
    JSONObjectImpl(const std::string& jsonStr) {}
    virtual ~JSONObjectImpl() {}

    virtual bool isArray() const = 0;
    
    virtual bool isReal() const = 0;

    virtual bool isInt() const = 0;

    virtual bool isString() const = 0;

    virtual bool isObject() const = 0;


    virtual bool hasKey(const std::string& key) const = 0;

    virtual std::shared_ptr<const JSONObject> get(const std::string& key) const = 0;

    virtual std::shared_ptr<const JSONObject> get(const int32_t& key) const = 0;
};



std::shared_ptr<JSONObject> ssr::nanoros::fromJSONString(const std::string& str) {
    return std::make_shared<JSONObjectImpl>(str);
}
