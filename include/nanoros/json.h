#pragma once

#include "nanoros/nanoros_define.h"

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
            virtual bool isBool() const = 0;
            virtual bool isArray() const = 0;
            virtual bool isReal() const = 0;
            virtual bool isInt() const = 0;
            virtual bool isString() const = 0;
            virtual bool isObject() const = 0;

            virtual operator int32_t() const = 0;
            virtual operator uint32_t() const = 0;
            virtual operator float() const = 0;
            virtual operator JSONIntType() const = 0;
            virtual operator JSONRealType() const = 0;
            virtual operator JSONStringType() const = 0;

            virtual bool hasKey(const std::string& key) const = 0;

            virtual std::shared_ptr<const JSONObject> get(const std::string& key) const = 0;

            virtual std::shared_ptr<const JSONObject> get(const int32_t& key) const = 0;

        protected:

        public:

            template<typename T>
            bool isType() const {
                return "nanoros/json.h line 50: Can not apply template method get to data type T.";
            }

            template<> bool isType<JSONIntType>() const { return isInt();}
            template<> bool isType<JSONRealType>() const { return isReal();}
            template<> bool isType<JSONStringType>() const { return isString();}
            template<> bool isType<JSONArrayType>() const { return isArray();}

            template<typename T>
            std::optional<T> get() const {
                if (isType<T>()) return static_cast<T>(*this);
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

        NANOROS_API std::shared_ptr<JSONObject> fromJSONString(const std::string& str);



    }
}