#include <map>
#include <string>
#include <sstream>
#include "nanoros/stringutil.h"
#include "nanoros/rossubscriber.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rosnode.h"

#include "nanoros/rosslaveserver.h"

using namespace ssr::nanoros;



class ROSSubscriberWorker {
public:
	ROSSubscriberWorker() {}
	virtual ~ROSSubscriberWorker() {}

public:
	virtual bool negotiateHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
		return false;
	}
	virtual void spinOnce() {}
};


class ROSSubscriberWorkerImpl: public ROSSubscriberWorker {
private:
	double receiveTimeout_;
	std::shared_ptr<TCPROS> tcpros_;
  const std::shared_ptr<ROSMsgStub> stub_;
  const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
public:
	ROSSubscriberWorkerImpl(const std::shared_ptr<ROSMsgStub>& stub, 
       const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback) : receiveTimeout_(1.0), stub_(stub), callback_(callback) {}
	virtual ~ROSSubscriberWorkerImpl() {}

public:
	
  virtual bool connect(const std::string& host, const int32_t port) {
	  tcpros_ = tcpros_connect(host, port);
	  return tcpros_ ? true : false;
  } 

  virtual bool disconnect() {
	  tcpros_disconnect(tcpros_);
	  tcpros_ = nullptr;
	  return true;
  }

  virtual bool sendHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
	if (!tcpros_) return false;
	return tcpros_->sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
  }

  virtual std::map<std::string, std::string> receiveHeader(const double timeout) {
	if (!tcpros_) return {};
	return tcpros_->receiveHeader(timeout);
	
  }

  virtual bool negotiateHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {		
	sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
	auto hdr = receiveHeader(timeout);
	if (hdr["topic"] != topicName) return false;
	return true; 
  }

  virtual void spinOnce() {
	if(tcpros_) {
		auto maybePacket = tcpros_->receivePacket(receiveTimeout_);
		auto maybeMsg = stub_->toMsg(maybePacket);
		if (maybeMsg) {
			callback_(maybeMsg);
		}
	}
  }
  
};


class ROSSubscriberImpl : public ROSSubscriber {
private:
  const std::string topicName_;
  std::vector<std::shared_ptr<ROSSubscriberWorker>> workers_;
  const std::shared_ptr<ROSMsgStub> stub_;
  const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
  ROSNode* node_;
public:
  const std::shared_ptr<ROSMsgStub>& stub() { return stub_; }
  ROSSubscriberImpl(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) : node_(node), topicName_(topicName), stub_(stub), callback_(func) {}

private:
  virtual std::shared_ptr<ROSSubscriberWorker> connect(const std::string& host, const int32_t port) {
	  auto worker = std::make_shared<ROSSubscriberWorkerImpl>(stub_, callback_);
	  if(worker->connect(host, port)) {
		  workers_.push_back(worker);
		  return std::static_pointer_cast<ROSSubscriberWorker>(worker);
	  }
	  return nullptr;
  } 


public:
  virtual bool connect(const std::string& uri, const bool latching=true, const double negotiateTimeout=1.0) { 
	auto slave = rosslave(uri);
	auto result = slave->requestTopic(node_->name(), topicName_, {ProtocolInfo(std::string("TCPROS"), node_->slaveServer()->getSlaveIP(), getEmptyPort(node_->getPortRange()))});
	if (!result) return false;
	if (result->code != 1) return false;

	auto worker = connect(result->protocolInfo.arg0, result->protocolInfo.arg1);
	if (!worker) { return false;
	}
	if (!worker->negotiateHeader(node_->name(), topicName_, stub_->typeName(), stub_->md5sum(), latching, negotiateTimeout)) return false;

	workers_.emplace_back(std::move(worker));	
	return true;
  }

  virtual ~ROSSubscriberImpl() {}

  virtual void spinOnce() {
	  for(auto& worker: workers_) {
		  worker->spinOnce();
	  }
  }

  
};


std::shared_ptr<ROSSubscriber> ssr::nanoros::createROSSubscriber(ROSNode* node, const std::string& topicName,
 const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) {
	return std::static_pointer_cast<ROSSubscriber>(std::make_shared<ROSSubscriberImpl>(node, topicName, stub, func));
}
