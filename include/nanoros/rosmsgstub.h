#pragma once

#include "nanoros/rosmsg.h"
#include "nanoros/json.h"
#include "nanoros/rostcpros.h"
#include <string>
#include <map>
#include <optional>

namespace ssr {
  namespace nanoros {

    template<typename T>
    void setValue(T& value, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
        if (data && data->isType<T>()) {
            value = data->get<T>().value();
        }
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





    class TCPROSPacket;

    class ROSMsgStub {
    protected:
      std::map<std::string, std::shared_ptr<ROSMsgStub>> stubs_;

    public:
      std::shared_ptr<ROSMsgStub> getStub(const std::string& key) const {
        if (stubs_.count(key) > 0) return stubs_.at(key);
        return nullptr;
      }
    public:
      ROSMsgStub() {}
      virtual ~ROSMsgStub() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }

      virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) const { 
        int32_t popedCount = 0;
        return toMsg(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) const { return nullptr; }
      virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) const {return nullptr; }
      virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) { return nullptr; }
    };


    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgStub>& stub, const std::shared_ptr<const ssr::nanoros::JSONObject>& data) {
      if (!stub) return;
      if (!data) return;

      std::shared_ptr<ROSMsg> val = stub->fromJSON(data);
      if (!val) return;
      auto v = std::dynamic_pointer_cast<T>(val);
      if (v) {
        value = *(v.get());
      }
    }

    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgStub>& stub, const std::shared_ptr<const ssr::nanoros::JSONObject>& obj, const std::string& key) {
        setValue<T>(value, stub, obj->get(key));
    }

    template<typename T>
    void setValue(T& value, const std::shared_ptr<ROSMsgStub>& stub, const std::optional<ssr::nanoros::TCPROSPacket>& msg, int32_t& popedCount) {
        if (!stub) return;
        auto rosMsg= stub->toMsg(msg, popedCount);
        if (!rosMsg) return;
        auto val = std::dynamic_pointer_cast<const T>(rosMsg);
        if (!val) return;
        value = *(val.get());
    }

    template<typename T>
    void pushValue(std::shared_ptr<TCPROSPacket>& pkt, const std::shared_ptr<ROSMsgStub>& stub, const T& value) {
      if (!pkt) return;
      if (!stub) return;
      //auto subPkt = stub->toPacket(std::make_shared<T>(value));
      auto subPkt = stub->toPacket(value);
      if (!subPkt) return;
      pkt->push(*(subPkt.get()));
    }

  }
}
