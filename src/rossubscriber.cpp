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
private:
	double receiveTimeout_;
	std::shared_ptr<TCPROS> tcpros_;
	std::string host_;
	int32_t port_;
  const std::shared_ptr<ROSMsgStub> stub_;
  const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
public:
	ROSSubscriberWorker(const std::shared_ptr<ROSMsgStub>& stub, 
       const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback) : receiveTimeout_(1.0), stub_(stub), callback_(callback) {}
	virtual ~ROSSubscriberWorker() {}

public:
  std::string getPublisherUri() const { 
	  return "tcpros://" + host_ + ":" + std::to_string(port_);
  }

  virtual bool connect(const std::string& host, const int32_t port) {
	  host_ = host;
	  port_ = port;
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
  std::vector<std::shared_ptr<ROSSubscriberWorker>> workers_;
  const std::shared_ptr<ROSMsgStub> stub_;
  const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
public:
	virtual std::string getTopicTypeName() const override { return stub_->typeName(); }
  const std::shared_ptr<ROSMsgStub>& stub() { return stub_; }
  ROSSubscriberImpl(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) : 
  	ROSSubscriber(node, topicName), stub_(stub), callback_(func) {}

private:
  virtual std::shared_ptr<ROSSubscriberWorker> connect(const std::string& host, const int32_t port) {
	  auto worker = std::make_shared<ROSSubscriberWorker>(stub_, callback_);
	  if(worker->connect(host, port)) {
		  workers_.push_back(worker);
		  return std::static_pointer_cast<ROSSubscriberWorker>(worker);
	  }
	  return nullptr;
  } 


public:
  virtual bool connect(const std::string& uri, const bool latching=true, const double negotiateTimeout=1.0) override { 
	  disconnectUri(uri);
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

  virtual bool disconnectUri(const std::string& uri) override {
	  for(auto it = workers_.begin(); it != workers_.end(); ++it) {
		  if ((*it)->getPublisherUri() == uri) {
			  disconnect();
			  workers_.erase(it);
			  return true;
		  }
	  }
	  return false;
  }
  virtual ~ROSSubscriberImpl() {}

  virtual void spinOnce() override {
	  for(auto& worker: workers_) {
		  worker->spinOnce();
	  }
  }

  virtual std::optional<std::vector<std::string>> getSubscribingPublisherUris() const override { 
	  std::vector<std::string> val;
	  for(auto worker: workers_) {
		  val.push_back(worker->getPublisherUri());
	  }
	  return val;
  }
  
};


std::shared_ptr<ROSSubscriber> ssr::nanoros::createROSSubscriber(ROSNode* node, const std::string& topicName,
 const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) {
	 if (!stub) return nullptr;
	return std::static_pointer_cast<ROSSubscriber>(std::make_shared<ROSSubscriberImpl>(node, topicName, stub, func));
}
