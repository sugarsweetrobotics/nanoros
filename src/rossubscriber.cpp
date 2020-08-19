#include <map>
#include <string>
#include <sstream>
#include "nanoros/stringutil.h"
#include "nanoros/rossubscriber.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rosnode.h"

#include "nanoros/rosutil.h"
#include "nanoros/rosslaveserver.h"

#include <mutex>
#include <iostream>


using namespace ssr::nanoros;

template<typename T>
std::vector<T> filter(const std::vector<T>& vals, const std::function<bool(const T&)>& func) {
  std::vector<T> retval;
  for(auto& val : vals) {
    if (func(val)) retval.push_back(val);
  }
  return retval;
}
  

class ROSSubscriberWorker {
private:
	double receiveTimeout_;
	std::shared_ptr<TCPROS> tcpros_;
	std::string host_;
	int32_t port_;

	std::mutex tcpros_mutex_;

    const std::shared_ptr<ROSMsgPacker> packer_;
    const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
public:
	ROSSubscriberWorker(const std::shared_ptr<ROSMsgPacker>& packer, 
       const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback) 
		: receiveTimeout_(1.0), packer_(packer), callback_(callback), port_(-1) {}
	virtual ~ROSSubscriberWorker() {}

public:
  std::string getPublisherUri() const { 
	  return "tcpros://" + host_ + ":" + std::to_string(port_);
  }

  virtual bool connect(const std::string& host, const int32_t port, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout = 1.0) {
	  std::cout << "ROSSubscriberWorker::connect(" << host << ", " << port << ")" << std::endl;
	  host_ = host;
	  port_ = port;
	  auto tcpros = tcpros_connect(host, port);
	  if (!tcpros) {
		  std::cout << " - connection failed." << std::endl;
		  return false;
	  }


	  if (!negotiateHeader(tcpros, caller_id, topicName, topicTypeName, md5sum, latching, timeout)) {
		  return false;
	  }

	  {
		  std::lock_guard<std::mutex> grd(tcpros_mutex_);
		  tcpros_ = tcpros;
	  }
	  return true;
  } 

  virtual bool disconnect() {
	  std::lock_guard<std::mutex> grd(tcpros_mutex_);
	  tcpros_disconnect(tcpros_);
	  tcpros_ = nullptr;
	  return true;
  }

  virtual bool sendHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
	  //std::lock_guard<std::mutex> grd(tcpros_mutex_);
	  if (!tcpros) return false;
      return tcpros->sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
  }

  virtual std::map<std::string, std::string> receiveHeader(std::shared_ptr<TCPROS>& tcpros, const double timeout) {
	  ///std::lock_guard<std::mutex> grd(tcpros_mutex_);
	  if (!tcpros) return {};
	return tcpros->receiveHeader(timeout);
	
  }

  virtual bool negotiateHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
	sendHeader(tcpros, caller_id, topicName, topicTypeName, md5sum, latching);
	auto hdr = receiveHeader(tcpros, timeout);
	if (hdr["topic"] != topicName) return false;
	return true; 
  }

  virtual bool spinOnce() {
	  std::lock_guard<std::mutex> grd(tcpros_mutex_);
	  if(tcpros_ ) {
		  if (!tcpros_->isConnected()) {
			  std::cout << " - Detecting SubscriptionWorker is not connected already." << std::endl;
			  tcpros_ = nullptr;
			  return false;
		  }
			auto maybePacket = tcpros_->receivePacket(receiveTimeout_);
			if (!maybePacket) {
				std::cout << " - maybePacket is invalid." << std::endl;
				return false;
			}
			auto maybeMsg = packer_->toMsg(maybePacket);
			if (maybeMsg) {
				callback_(maybeMsg);
				return true;
			}
      }
	  return true;
  }
  
};


class ROSSubscriberImpl : public ROSSubscriber {
private:
	std::mutex workers_mutex_;
  std::vector<std::shared_ptr<ROSSubscriberWorker>> workers_;
  std::shared_ptr<ROSMsgPacker> packer_;
  const std::function<void(const std::shared_ptr<const ROSMsg>& msg)> callback_;
public:
	virtual std::string getTopicTypeName() const override { return packer_->typeName(); }
  const std::shared_ptr<ROSMsgPacker>& packer() { return packer_; }


  ROSSubscriberImpl(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) 
  : ROSSubscriber(node, topicName), packer_(packer), callback_(func) {
	  if (!packer_) {
		  /// if (packer_ is null), wait publisherUpdate callback to specify topic type.
	  }
  }

private:
  virtual std::shared_ptr<ROSSubscriberWorker> connect(const std::string& host, const int32_t port, bool latching, const double negotiateTimeout) {
	  std::cout << "ROSSbuscriberImpl::connect(" << host << ", " << port << ")" << std::endl;
	  if (!packer_) {
		  // if packer_ is null, connection should not be done.
		  auto types = this->node_->master()->getTopicTypes(this->node_->name());
		  auto topicName = this->getTopicName();
		  if (!types) { 
			  std::cout << " - TopicTypes can not be found." << std::endl;
			  return nullptr;
		  }
		  auto vs = filter<TopicTypeInfo>(types->topicTypes, [topicName](const TopicTypeInfo& v) { return v.topicName == topicName; });
		  if (vs.size() == 0) { 
			  std::cout << " - Appropriate topic(" << topicName << ") not found." << std::endl;
     		  return nullptr;
		  }
		  auto topicTypeName = vs[0].topicType;
		  auto packer = getROSMsgPackerFactory()->getPacker(topicTypeName);
          if (!packer) {
			  std::cout << " - Packer for topicType(" << topicTypeName << ") can not load" << std::endl;
			  return nullptr;
		  }
		  packer_ = packer;
	  }
	  auto worker = std::make_shared<ROSSubscriberWorker>(packer_, callback_);
	  if(worker->connect(host, port, node_->name(), topicName_, packer_->typeName(), packer_->md5sum(), latching, negotiateTimeout)) {
		  //if (!worker->negotiateHeader(node_->name(), topicName_, packer_->typeName(), packer_->md5sum(), latching, negotiateTimeout)) {
			//  return nullptr;
		  //}

		  return std::static_pointer_cast<ROSSubscriberWorker>(worker);
	  }
	  std::cout << " - connect worker faild." << std::endl;
	  return nullptr;
  } 


public:
  virtual bool connect(const std::string& uri, const bool latching=true, const double negotiateTimeout=1.0) override { 
	  disconnectUri(uri);
	auto slave = rosslave(uri);
	auto result = slave->requestTopic(node_->name(), topicName_, {ProtocolInfo(std::string("TCPROS"), node_->slaveServer()->getSlaveIP(), getEmptyPort(node_->getPortRange()))});
	if (!result) return false;
	if (result->code != 1) return false;

	auto worker = connect(result->protocolInfo.arg0, result->protocolInfo.arg1, latching, negotiateTimeout);
	if (!worker) {
		return false;
	}
	{
		std::lock_guard<std::mutex> grd(workers_mutex_);
		workers_.push_back(worker);
	}
	return true;
  }

  virtual bool disconnectUri(const std::string& uri) override {
	  std::lock_guard<std::mutex> grd(workers_mutex_);
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

	  std::lock_guard<std::mutex> grd(workers_mutex_);
	  for (auto it = workers_.begin(); it != workers_.end(); ) {
		  auto& worker = *it;
		  if (!worker->spinOnce()) {
			  it = workers_.erase(it);
		  }
		  else {
			  ++it;
		  }
	  }
  }

  virtual std::optional<std::vector<std::string>> getSubscribingPublisherUris() override { 
	  std::vector<std::string> val;
	  std::lock_guard<std::mutex> grd(workers_mutex_);

	  for(auto worker: workers_) {
		  val.push_back(worker->getPublisherUri());
	  }
	  return val;
  }
  
};


std::shared_ptr<ROSSubscriber> ssr::nanoros::createROSSubscriber(ROSNode* node, const std::string& topicName,
 const std::shared_ptr<ROSMsgPacker>& packer, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func) {
	// if (!packer) return nullptr;
	return std::static_pointer_cast<ROSSubscriber>(std::make_shared<ROSSubscriberImpl>(node, topicName, packer, func));
}
