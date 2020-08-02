#include "nanoros/rosmasterserver.h"



#include "XmlRpc/XmlRpc.h"
#include "XmlRpc/XmlRpcServer.h"


#include <thread>

using namespace ssr::nanoros;

using namespace XmlRpc;


class ROSMasterServerImpl;

class ROSMasterMethod : public XmlRpcServerMethod {
protected:
  ROSMasterServerImpl* impl_;
public:
  ROSMasterMethod(const std::string& name, class ROSMasterServerImpl * si);
  virtual ~ROSMasterMethod() {}

};

class RegisterService : public ROSMasterMethod {
public:
  RegisterService(class ROSMasterServerImpl * si) : ROSMasterMethod("registerServer", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};

class UnregisterService : public ROSMasterMethod {
public:
  UnregisterService(class ROSMasterServerImpl * si) : ROSMasterMethod("unregisterServer", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};

class RegisterPublisher : public ROSMasterMethod {
public:
  RegisterPublisher(class ROSMasterServerImpl * si) : ROSMasterMethod("registerPublisher", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};

class UnregisterPublisher : public ROSMasterMethod {
public:
  UnregisterPublisher(class ROSMasterServerImpl * si) : ROSMasterMethod("unregisterPublisher", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};

class RegisterSubscriber : public ROSMasterMethod {
public:
  RegisterSubscriber(class ROSMasterServerImpl * si) : ROSMasterMethod("registerSubscriber", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};

class UnregisterSubscriber : public ROSMasterMethod {
public:
  UnregisterSubscriber(class ROSMasterServerImpl * si) : ROSMasterMethod("unregisterSubscriber", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};




class LookupNode : public ROSMasterMethod {
public:
  LookupNode(class ROSMasterServerImpl * si) : ROSMasterMethod("lookupNode", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};


class GetPublishedTopics : public ROSMasterMethod {
public:
  GetPublishedTopics(class ROSMasterServerImpl * si) : ROSMasterMethod("getPublishedTopics", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};


class GetTopicTypes : public ROSMasterMethod {
public:
  GetTopicTypes(class ROSMasterServerImpl * si) : ROSMasterMethod("getTopicTypes", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};


class GetSystemState : public ROSMasterMethod {
public:
  GetSystemState(class ROSMasterServerImpl * si) : ROSMasterMethod("getSystemState", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};


class GetUri : public ROSMasterMethod {
public:
  GetUri(class ROSMasterServerImpl * si) : ROSMasterMethod("getUri", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};


class LookupService : public ROSMasterMethod {
public:
  LookupService(class ROSMasterServerImpl * si) : ROSMasterMethod("lookupService", si) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result) {
      
  }
};



class ROSMasterServerImpl : public ROSMasterServer {
public:

  const std::shared_ptr<XmlRpcServer> server_;
    bool exit_flag_;
    std::shared_ptr<std::thread> thread_;

    RegisterService registerService_;
    UnregisterService unregisterService_;
    RegisterPublisher registerPublisher_;
    UnregisterPublisher unregisterPublisher_;
    RegisterSubscriber registerSubscriber_;
    UnregisterSubscriber unregisterSubscriber_;
    LookupNode lookupNode_;
    GetPublishedTopics getPublishedTopics_;
    GetTopicTypes getTopicTypes_;
    GetSystemState getSystemState_;
    GetUri getUri_;
    LookupService lookupService_;
public: 
    ROSMasterServerImpl(const std::string& addr, const int32_t port) :
    server_(std::make_shared<XmlRpcServer>()),
    registerService_(this),
    unregisterService_(this),
    registerPublisher_(this),
    unregisterPublisher_(this),
    registerSubscriber_(this),
    unregisterSubscriber_(this),
    lookupNode_(this),
    getPublishedTopics_(this),
    getTopicTypes_(this),
    getSystemState_(this),
    getUri_(this),
    lookupService_(this)
    {}

    virtual ~ROSMasterServerImpl() {}



public:
    virtual void start(const bool blocking = true) {


        /*
        exit_flag_ = false;
        thread_ = std::make_shared<std::thread>([this]() {
        server_->bindAndListen(port_);
        server_->enableIntrospection(true);
        while(!exit_flag_) {
            server_->work(1.0);
        }
        });
        */
    }
};


std::shared_ptr<ROSMasterServer> ssr::nanoros::masterServer(std::optional<std::pair<std::string, int32_t>>& info) {
    if (!info) return std::make_shared<ROSMasterServer>();
    return std::make_shared<ROSMasterServerImpl>(info->first, info->second);
}


ROSMasterMethod::ROSMasterMethod(const std::string& name, class ROSMasterServerImpl * si)  : XmlRpcServerMethod(name, si->server_.get()), impl_(si) {
}  
