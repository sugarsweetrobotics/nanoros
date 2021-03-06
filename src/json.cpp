#include "nanoros/json.h"
#include <nlohmann/json.hpp>

using namespace ssr::nanoros;
using json = nlohmann::json;

class JSONObjectImpl : public JSONObject {
private:
    json json_;
public:
    JSONObjectImpl(const std::string& jsonStr) : json_(json::parse(jsonStr)) {
    }

    JSONObjectImpl(const json& jsn) : json_(jsn) {}

    JSONObjectImpl(json&& jsn): json_(std::move(jsn)) {}

    virtual ~JSONObjectImpl() {}

    virtual bool isBool() const override { 
        return json_.is_boolean();
    }

    virtual bool isArray() const override {
        return json_.is_array();
    }

    virtual int arraySize() const override {
        return json_.array().size();
    }

    virtual bool isReal() const override {
        return json_.is_number();
    }

    virtual bool isInt() const override {
        return json_.is_number();
    }

    virtual bool isString() const override {
        return json_.is_string();
    }

    virtual bool isObject() const override {
        return json_.is_object();
    }

    virtual operator int8_t() const override {
        return json_.get<int8_t>();
    }

    virtual operator char() const override {
        return json_.get<char>();
    }

    virtual operator uint8_t() const override {
        return json_.get<uint8_t>();
    }

    virtual operator int16_t() const override {
        return json_.get<int16_t>();
    }

    virtual operator uint16_t() const override {
        return json_.get<uint16_t>();
    }

    virtual operator int32_t() const override {
        return json_.get<int32_t>();
    }

    virtual operator uint32_t() const override {
        return json_.get<uint32_t>();
    }
    
    //virtual operator int64_t() const override {
    //    return json_.get<int64_t>();
    //}

    virtual operator uint64_t() const override {
        return json_.get<uint64_t>();
    }

    virtual operator float() const override {
        return json_.get<float>();
    }
   
    //virtual operator double() const override {
    //    return json_.get<double>();
    //}

    virtual operator JSONIntType() const override {
        return json_.get<int64_t>();
    }

    virtual operator JSONRealType() const override {
        return json_.get<JSONRealType>();
    }

    virtual operator JSONStringType() const override {
        return json_.get<JSONStringType>();
    }


    virtual bool hasKey(const std::string& key) const  override {
        if (!isObject()) return false;
        return json_.contains(key);
    }

    virtual std::shared_ptr<const JSONObject> get(const std::string& key) const override {
        if (!isObject()) return nullptr;
        return std::make_shared<JSONObjectImpl>(json_.at(key));
    }

    virtual std::shared_ptr<const JSONObject> get(const int32_t& key) const override {
        if (!isArray()) return nullptr;
        return std::make_shared<JSONObjectImpl>(json_.at(key));
    }

    virtual std::string prettyString(const std::string& indent) const override {
        int len = indent.length();
        return json_.dump(len);
    }
};



std::shared_ptr<JSONObject> ssr::nanoros::fromJSONString(const std::string& str) {
    return std::make_shared<JSONObjectImpl>(str);
}
