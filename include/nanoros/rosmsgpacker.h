#pragma once

#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsg.h"
#include "nanoros/json.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rostcprospacket.h"
#include "nanoros/rosmsgpackerfactory.h"

#include <string>
#include <map>
#include <optional>

namespace ssr {
  namespace nanoros {

    class ROSMsgPackerFactory;

    NANOROS_API std::shared_ptr<ROSMsgPackerFactory> getROSMsgPackerFactory();

    class TCPROSPacket;

    NANOROS_API class ROSMsgPacker;

    ///NANOROS_API std::map<std::string, std::shared_ptr<ROSMsgPacker>>;

    class NANOROS_API ROSMsgPacker {
    protected:
      friend class NANOROS_API std::map<std::string, std::shared_ptr<ROSMsgPacker>>;
      std::map<std::string, std::shared_ptr<ROSMsgPacker>> packers_;

    public:
      std::shared_ptr<ROSMsgPacker> getMsgPacker(const std::string& key);

    public:
      ROSMsgPacker() {}
      virtual ~ROSMsgPacker() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }

      virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) { 
        int32_t popedCount = 0;
        return toMsg(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) { return nullptr; }

      virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) {return nullptr; }

      virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) { return nullptr; }
    };


    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
      if (!packer) return;
      if (!data) return;

      std::shared_ptr<ROSMsg> val = packer->fromJSON(data);
      if (!val) return;
      auto v = std::dynamic_pointer_cast<T>(val);
      if (v) {
        value = *(v.get());
      }
    }

    template<typename T>
    void setArrayValue(std::vector<T>& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
        if (!packer) return;
        if (!data) return;
        if (!data->isArray()) return;
        for (int i = 0; i < data->arraySize(); i++) {
            auto e = data->get(i);
            std::shared_ptr<ROSMsg> val = packer->fromJSON(e);
            if (!val) return;
            auto v = std::dynamic_pointer_cast<T>(val);
            if (v) {
                value.push_back(*(v.get()));
            }
        }
    }

    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::shared_ptr<const ssr::nanoros::JSONObject>& obj, const std::string& key) {
        setValue<T>(value, packer, obj->get(key));
    }

    template<typename T>
    void setArrayValue(std::vector<T>& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::shared_ptr<const ssr::nanoros::JSONObject>& obj, const std::string& key) {
        setArrayValue<T>(value, packer, obj->get(key));
    }

    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::optional<ssr::nanoros::TCPROSPacket>& msg, int32_t& popedCount) {
        if (!packer) return;
        auto rosMsg= packer->toMsg(msg, popedCount);
        if (!rosMsg) return;
        auto val = std::dynamic_pointer_cast<const T>(rosMsg);
        if (!val) return;
        value = *(val.get());
    }


    template<typename T>
    void pushValue(std::vector<T>& value, const std::optional<ssr::nanoros::TCPROSPacket>& msg, int32_t& popedCount) {
        auto size = msg->pop<uint32_t>(popedCount);
        if (!size) return;
        for (int i = 0; i < size.value(); i++) {
            auto val = msg->pop<T>(popedCount);
            if (!val) return;
            value.push_back(val.value());
        }
    }

    template<typename T>
    void pushValue(std::vector<T>& value, const std::shared_ptr<ROSMsgPacker>& packer, const std::optional<ssr::nanoros::TCPROSPacket>& msg, int32_t& popedCount) {
        if (!packer) return;
        auto size = msg->pop<uint32_t>(popedCount);
        if (!size) return;
        for(int i = 0;i < size.value();i++) {
          auto rosMsg= packer->toMsg(msg, popedCount);
          if (!rosMsg) return;
          auto val = std::dynamic_pointer_cast<const T>(rosMsg);
          if (!val) return;
          value.push_back(*val.get());
        }
    }

    template<typename T>
    void pushValue(std::shared_ptr<TCPROSPacket>& pkt, const std::shared_ptr<ROSMsgPacker>& packer, const T& value) {
      if (!pkt) return;
      if (!packer) return;
      //auto subPkt = packer->toPacket(std::make_shared<T>(value));
      auto subPkt = packer->toPacket(value);
      if (!subPkt) return;
      pkt->push(*(subPkt.get()));
    }

    template<typename T>
    void pushVectorValue(std::shared_ptr<TCPROSPacket>& pkt, const std::shared_ptr<ROSMsgPacker>& packer, const std::vector<T>& value) {
        if (!pkt) return;
        if (!packer) return;
        uint32_t size = value.size();
        pkt->push<uint32_t>(size);
        for (auto& e : value) {
            auto subPkt = packer->toPacket(e);
            if (!subPkt) return;
            pkt->push(*(subPkt.get()));
        }
    }



    template<typename T>
    void setValue(T& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
        if (data && data->isType<T>()) {
            value = data->get<T>().value();
        }
    }

    template<typename T>
    void setArrayValue(std::vector<T>& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
        if (data && data->isArray()) {
            for (int i = 0; i < data->arraySize(); i++) {
                auto e = data->get(i);
                value.push_back(e->get<T>().value());
            }
        }
    }


    template<typename T>
    void setArrayValue(std::vector<T>& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& obj, const std::string& key) {
        setArrayValue<T>(value, obj->get(key));
    }

    template<typename T>
    void setValue(T& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& obj, const std::string& key) {
        setValue<T>(value, obj->get(key));
    }


    template<>
    inline void setValue<ssr::nanoros::time>(ssr::nanoros::time& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
        if (data && data->isObject()) {
          setValue<uint32_t>(value.sec, data, "sec");
          setValue<uint32_t>(value.nsec, data, "nsec");
        }
    }

    template<typename V, typename T>
    void setValue(std::shared_ptr<V>& value, T& v, const std::optional<T>& data) {
      if (!data) {
        return;
      }
      if (!data) {
        value = nullptr;
      }
      v = data.value();
    }


    
  }
}
