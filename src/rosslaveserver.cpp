
#include <thread>

#include "nanoros/rosslaveserver.h"
#include "nanoros/rosnode.h"


#include "XmlRpc/XmlRpc.h"
#include "XmlRpc/XmlRpcServer.h"

using namespace ssr::nanoros;
using namespace XmlRpc;

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


class ROSSlaveServerImpl : public ROSSlaveServer {
private:
  const std::shared_ptr<ROSMaster> master_;
  const std::string ip_;
  const int32_t port_;
  const std::shared_ptr<XmlRpcServer> server_;
  ROSNode* rosnode_;
  std::shared_ptr<std::thread> thread_;
  GetBusInfo getBusInfo_;
  GetBusInfo getBusStats_;
  GetMasterUri getMasterUri_;
public:
  ROSSlaveServerImpl(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& ip, const int32_t port): 
    rosnode_(rosnode), 
    master_(master), ip_(ip), 
    port_(port), server_(std::make_shared<XmlRpcServer>()),
    getBusInfo_(server_.get()),
    getBusStats_(server_.get()),
    getMasterUri_(server_.get())
  {


    thread_ = std::make_shared<std::thread>([this]() {
      server_->bindAndListen(port_);
      server_->enableIntrospection(true);
      server_->work(-1.0);
    });
  }

  virtual ~ROSSlaveServerImpl() {
    server_->exit();
    thread_->join();
  }


  virtual std::string getSlaveUri() const {
    return "http://" + ip_ + ":" + std::to_string(port_);
  }

  virtual std::string getSlaveIP() const {
    return ip_;
  }
};

std::shared_ptr<ROSSlaveServer> ssr::nanoros::rosslaveserver(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& ip, const int32_t port) {
  return std::static_pointer_cast<ROSSlaveServer>(std::make_shared<ROSSlaveServerImpl>(rosnode, master, ip, port));
}
