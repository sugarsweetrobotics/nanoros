#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace ssr {
    namespace nanoros {
        class JSONObject;

        using JSONIntType = int64_t;
        using JSONRealType = double;
        using JSONArrayType = std::vector<std::shared_ptr<JSONObject>>;
        using JSONObjectType = JSONObject;
        using JSONStringType = std::string;

        class JSONObject {
        public:
            virtual bool isArray() const = 0;
            virtual bool isReal() const = 0;
            virtual bool isInt() const = 0;
            virtual bool isString() const = 0;
            virtual bool isObject() const = 0;

            operator JSONIntType() const { return *value_.intValue; }
            operator JSONRealType() const { return *value_.realValue; }
            operator JSONStringType() const { return *value_.stringValue; }
            operator JSONArrayType() const { return *value_.arrayValue; }

            

            virtual bool hasKey(const std::string& key) const = 0;
            virtual std::shared_ptr<const JSONObject> get(const std::string& key) const = 0;

            virtual std::shared_ptr<const JSONObject> get(const int32_t& key) const = 0;

        protected:

        public:



            template<typename T>
            std::optional<T> get() const {
                return "nanoros/json.h line 50: Can not apply template method get to data type T.";
                //return std::nullopt;
            }

            template<>
            std::optional<JSONIntType> get<JSONIntType>() const {
                if (isInt()) return static_cast<const JSONIntType>(*this);
                return std::nullopt;
            }

            template<>
            std::optional<JSONRealType> get<JSONRealType>() const {
                if (isReal()) return static_cast<const JSONRealType>(*this);
                return std::nullopt;
            }

            template<>
            std::optional<JSONStringType> get<JSONStringType>() const {
                if (isString()) return static_cast<const JSONStringType>(*this);
                return std::nullopt;
            }

            friend JSONObject jsonArray();
            friend JSONObject jsonObject();
            friend JSONObject jsonString(const JSONStringType& str);
            friend JSONObject jsonInt(const JSONIntType& intval);
            friend JSONObject jsonReal(const JSONRealType& realval);
       
        public:
            JSONObject() {}
            virtual ~JSONObject() {}
        };

        std::shared_ptr<JSONObject> fromJSONString(const std::string& str);



    }
}