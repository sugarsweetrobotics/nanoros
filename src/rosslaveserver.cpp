
#include <thread>

#include "nanoros/rosslaveserver.h"
#include "nanoros/rosnode.h"


#include "XmlRpc/XmlRpc.h"
#include "XmlRpc/XmlRpcServer.h"

using namespace ssr::nanoros;
using namespace XmlRpc;

const int32_t port_base = 30000;


class ROSSlaveServerImpl;

class GetBusStats : public XmlRpcServerMethod {
public:
  GetBusStats(XmlRpcServer* s) : XmlRpcServerMethod("getBusStats", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {

  }
};
class GetBusInfo : public XmlRpcServerMethod {
public:
  GetBusInfo(XmlRpcServer* s) : XmlRpcServerMethod("getBusInfo", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {

  }
};
class GetMasterUri : public XmlRpcServerMethod {
public:
  GetMasterUri(XmlRpcServer* s) : XmlRpcServerMethod("getMasterUri", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {

  }
};
class RequestTopic : public XmlRpcServerMethod {
private:
  ROSSlaveServerImpl* slaveServerImpl_;
public:
  RequestTopic(class ROSSlaveServerImpl * si);

  void execute(XmlRpcValue& params, XmlRpcValue& result);
};


class ROSSlaveServerImpl : public ROSSlaveServer {
public:
  const std::shared_ptr<ROSMaster> master_;
  const std::string ip_;
  const int32_t port_;
  const std::shared_ptr<XmlRpcServer> server_;
  ROSNode* rosnode_;
  std::shared_ptr<std::thread> thread_;
  GetBusInfo getBusInfo_;
  GetBusInfo getBusStats_;
  GetMasterUri getMasterUri_;
  RequestTopic requestTopic_;

  bool exit_flag_;

public:
  ROSSlaveServerImpl(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& ip, const int32_t port): 
    rosnode_(rosnode), 
    master_(master), ip_(ip), 
    port_(port), server_(std::make_shared<XmlRpcServer>()),
    getBusInfo_(server_.get()),
    getBusStats_(server_.get()),
    getMasterUri_(server_.get()),
    requestTopic_(this)
  {

    exit_flag_ = false;
    thread_ = std::make_shared<std::thread>([this]() {
      server_->bindAndListen(port_);
      server_->enableIntrospection(true);
      while(!exit_flag_) {
        server_->work(1.0);
      }
    });
  }

  virtual ~ROSSlaveServerImpl() {
    server_->shutdown();
    exit_flag_ = true;
    thread_->join();
  }


  virtual std::string getSlaveUri() const override {
    return "http://" + ip_ + ":" + std::to_string(port_);
  }

  virtual std::string getSlaveIP() const override  {
    return ip_;
  }


  virtual bool standByNode(const std::string& topicName, const std::string& caller_id, const std::string& selfIP, const int32_t port) override {
    auto pub = rosnode_->getPublisher(topicName);
    if (!pub) return false;

    return pub->standBy(caller_id, selfIP, port);
  }
};

std::shared_ptr<ROSSlaveServer> ssr::nanoros::rosslaveserver(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& ip, const int32_t port) {
  return std::static_pointer_cast<ROSSlaveServer>(std::make_shared<ROSSlaveServerImpl>(rosnode, master, ip, port));
}


RequestTopic::RequestTopic(ROSSlaveServerImpl* si) : XmlRpcServerMethod("requestTopic", si->server_.get()), slaveServerImpl_(si) {

}  

void RequestTopic::execute(XmlRpcValue& params, XmlRpcValue& result) {
  const std::string caller_id = params[0];
  const std::string topicName = params[1];
  if (params[2].getType() != XmlRpcValue::TypeArray) {
    return;
  }
  for(int i = 0;i < params[2].size();i++) {
    const std::string protocolName = params[2][i][0];
    std::vector<std::string> args;
    size_t size = params[2][i].size();
    for(int j = 1;j < size;j++) {
      if (params[2][i][j].getType() == XmlRpcValue::TypeString) {
        args.push_back(params[2][i][j]);
      }
      else if (params[2][i][j].getType() == XmlRpcValue::TypeInt) {
        args.push_back(std::to_string((int)(params[2][i][j])));
      }
    }

    if (protocolName == "TCPROS") {
      const std::string selfIP = getSelfIP();
      auto port = getEmptyPort(port_base);
      if (!slaveServerImpl_->standByNode(topicName, caller_id, selfIP, port)) continue;
      const std::string portStr = std::to_string(port);
      result[0] = 1;
      result[1] = "Publisher is waiting on uri:" + selfIP + ":" + portStr;
      result[2][0] = "TCPROS";
      result[2][1] = selfIP;
      result[2][2] = port;
      return;
    }

      
  }
  result[0] = -1;
  result[1] = "None of suggested protocol is available in publisher.";
}
