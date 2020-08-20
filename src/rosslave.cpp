#include "nanoros/rosslave.h"
#include "nanoros/rosutil.h"

#include "XmlRpc/XmlRpc.h"
#include "XmlRpc/XmlRpcUtil.h"

#include <iostream>
#include "plog/Log.h"

using namespace ssr::nanoros;

// Clear the referenced flag even if exceptions or errors occur.
struct ClearFlagOnExit {
  ClearFlagOnExit(bool& flag) : _flag(flag) {}
  ~ClearFlagOnExit() { _flag = false; }
  bool& _flag;
};


class MyXmlRpcClient: public XmlRpc::XmlRpcClient {
public:
    MyXmlRpcClient(const char* host, int port, const char* uri=0) : XmlRpc::XmlRpcClient(host, port, uri) {}

    //! Destructor
    virtual ~MyXmlRpcClient() {}

public:

  bool execute(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result, double msTime) {
    XmlRpc::XmlRpcUtil::log(1, "XmlRpcClient::execute: method %s (_connectionState %d).", method, _connectionState);

    // This is not a thread-safe operation, if you want to do multithreading, use separate
    // clients for each thread. If you want to protect yourself from multiple threads
    // accessing the same client, replace this code with a real mutex.
    if (_executing)
      return false;

    _executing = true;
    ClearFlagOnExit cf(_executing);

    _sendAttempts = 0;
    _isFault = false;

    if ( ! setupConnection())
      return false;

    if ( ! generateRequest(method, params))
      return false;

    result.clear();
    //double msTime = -1.0;   // Process until exit is called
    _disp.work(msTime);

    if (_connectionState != IDLE || ! parseResponse(result))
      return false;

    XmlRpc::XmlRpcUtil::log(1, "XmlRpcClient::execute: method %s completed.", method);
    _response = "";
    return true;
  }

};


class ROSSlaveImpl : public ROSSlave {
private:
  const std::string ip_;
  const int32_t port_;
  MyXmlRpcClient client_;
  double xmlrpc_timeout_;
public:
  ROSSlaveImpl(const std::string& ip, const int32_t port) : ip_(ip), port_(port), client_(ip.c_str(), port), xmlrpc_timeout_(3.0) {}
  virtual ~ROSSlaveImpl() {}


  virtual std::optional<MasterMsg> publisherUpdate(const std::string& caller_id, const std::string& topic_name, const std::vector<std::string>& publishers) {
    PLOGV << "ROSSlaveImpl::publisherUpdate(" <<caller_id << ", " << topic_name << ", ...)";
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topic_name;
    v[2] = XmlRpc::XmlRpcValue();
    v[2].setSize(publishers.size());
    for (uint32_t i = 0; i < publishers.size(); ++i) {
      XmlRpc::XmlRpcValue vv;
      vv = publishers[i];
      v[2][i] = vv;
    }
    
    PLOGD << "ROSSlaveImpl::publisherUpdate(): Calling XmlRpcClient::execute(v=" << static_cast<std::string>(v) << ")";
    if (client_.execute("publisherUpdate", v, result, xmlrpc_timeout_)) {
          if (result.getType() != XmlRpc::XmlRpcValue::TypeArray) {
	    PLOGE << "ROSSlaveImpl::publisherUpdate() failed. Return value is not array type(result=" << static_cast<std::string>(result);
	    return std::nullopt;
          }
	  PLOGD << "ROSSlaveImpl::publisherUpdate() OK.  result=" << static_cast<std::string>(result);	  
          return MasterMsg(result[0], result[2]);
      }
      return std::nullopt;
  }

  virtual std::optional<RequestTopicResult> requestTopic(const std::string& caller_id,
                const std::string& topicName,
                const std::vector<ProtocolInfo>& info) 
  {
    PLOGV << "ROSSlaveImpl::requestTopic(" <<caller_id << ", " << topic_name << ", ...)";    
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = XmlRpc::XmlRpcValue();
    for(uint32_t i = 0;i < info.size();++i) {
      XmlRpc::XmlRpcValue pi;
      auto p = info[i];
      pi[0] = p.name;
      pi[1] = p.arg0;
      pi[2] = p.arg1;
      v[2][i] = pi;
    }
    PLOGD << "ROSSlaveImpl::requestTopic(): Calling XmlRpcClient::execute(v=" << static_cast<std::string>(v) << ")";    
    if (client_.execute("requestTopic", v, result, xmlrpc_timeout_)) {
        int32_t port = -1;
        if (result[2][2].getType() == XmlRpc::XmlRpcValue::TypeString) {
            port = std::atoi(static_cast<std::string>(result[2][2]).c_str());
        }
        else if (result[2][2].getType() == XmlRpc::XmlRpcValue::TypeInt) {
            port = result[2][2];
        }
        else {
	  PLOGE << "ROSSlaveImpl::requestTopic() failed. Return value is invalid. Port value must be string or integer. (result=" << static_cast<std::string>(result);	  
            return std::nullopt;
        }
      return RequestTopicResult((int)result[0], result[1], ProtocolInfo(result[2][0], result[2][1], port));
    }
    return std::nullopt;
  }
};

std::shared_ptr<ROSSlave> ssr::nanoros::rosslave(const std::string& publisherUri) {
  auto info = splitUri(publisherUri);
  return std::static_pointer_cast<ROSSlave>(std::make_shared<ROSSlaveImpl>(info->first, info->second));
}
