#pragma once

#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsg.h"
#include "nanoros/json.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rostcprospacket.h"
#include "nanoros/rosmsgstubfactory.h"

#include <string>
#include <map>
#include <optional>

namespace ssr {
  namespace nanoros {

    class ROSMsgStubFactory;

    NANOROS_API std::shared_ptr<ROSMsgStubFactory> getROSMsgStubFactory();

    class TCPROSPacket;

    NANOROS_API class ROSMsgStub;

    ///NANOROS_API std::map<std::string, std::shared_ptr<ROSMsgStub>>;

    class NANOROS_API ROSMsgStub {
    protected:
      friend class NANOROS_API std::map<std::string, std::shared_ptr<ROSMsgStub>>;
      std::map<std::string, std::shared_ptr<ROSMsgStub>> stubs_;

    public:
      std::shared_ptr<ROSMsgStub> getMsgStub(const std::string& key);

    public:
      ROSMsgStub() {}
      virtual ~ROSMsgStub() {}

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
    void pushValue(std::vector<T>& value, const std::shared_ptr<ROSMsgStub>& stub, const std::optional<ssr::nanoros::TCPROSPacket>& msg, int32_t& popedCount) {
        if (!stub) return;
        auto size = msg->pop<uint32_t>(popedCount);
        if (!size) return;
        for(int i = 0;i < size.value();i++) {
          auto rosMsg= stub->toMsg(msg, popedCount);
          if (!rosMsg) return;
          auto val = std::dynamic_pointer_cast<const T>(rosMsg);
          if (!val) return;
          value.push_back(*val.get());
        }
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

    
  }
}
