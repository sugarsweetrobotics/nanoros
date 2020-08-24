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

#include <plog/Log.h>

using namespace ssr::nanoros;

template<typename T>
std::vector<T> filter(const std::vector<T>& vals, const std::function<bool(const T&)>& func) {
  std::vector<T> retval;
  for(auto& val : vals) {
    if (func(val)) retval.push_back(val);
  }
  return retval;
}



void ROSSubscriber::spinOnce() {
  PLOGV << "ROSSubscriber::spinOnce() called";
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
		: receiveTimeout_(1.0), packer_(packer), callback_(callback), port_(-1) {
		PLOGV << "ROSSubscriberWorker::ROSSubscriberWorker(" << packer->typeName() << ") called." ;
	}


	virtual ~ROSSubscriberWorker() {}

public:
	std::string getPublisherUri() const { 
		return "tcpros://" + host_ + ":" + std::to_string(port_);
	}

	virtual bool connect(const std::string& host, const int32_t port, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout = 1.0) {
		PLOGV << "ROSSubscriberWorker::connect(" << host << ", " << port << ")" ;
		host_ = host;
		port_ = port;
		auto tcpros = tcpros_connect(host, port);
		if (!tcpros) {
			PLOGE << "ROSSubscriberWorker::connect(" << host << ", " << port << ") failed. TCPROS connect failed." ;
			return false;
		}


		if (!negotiateHeader(tcpros, caller_id, topicName, topicTypeName, md5sum, latching, timeout)) {
			PLOGE << "ROSSubscriberWorker::connect(" << host << ", " << port << ") failed. negotiateHeader failed." ;
			return false;
		}

		{
			std::lock_guard<std::mutex> grd(tcpros_mutex_);
			tcpros_ = tcpros;
		}
		return true;
	} 

	virtual bool disconnect() {
		PLOGV << "ROSSubscriberWorker::disconnect() called." ;
		std::lock_guard<std::mutex> grd(tcpros_mutex_);
		tcpros_disconnect(tcpros_);
		tcpros_ = nullptr;
		return true;
	}

	virtual bool sendHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
		PLOGV << "ROSSubscriberWorker::sendHeader(" << caller_id << ", " << topicName << ", " << topicTypeName << ", " << md5sum << ", " << latching << ") called." ;
		if (!tcpros) {
			PLOGE << "ROSSubscriberWorker::sendHeader(" << caller_id << ", " << topicName << ", " << topicTypeName << ", " << md5sum << ", " << latching << ") failed. TCPROS connection is nullptr" ;
			return false;
		}
		return tcpros->sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
	}

	virtual std::map<std::string, std::string> receiveHeader(std::shared_ptr<TCPROS>& tcpros, const double timeout) {
		PLOGV << "ROSSubscriberWorker::receiveHeader() called." ;
		if (!tcpros) {
			PLOGE << "ROSSubscriberWorker::receiveHeader() failed. TCPROS connection is nullptr" ;
			return {};
		}
		return tcpros->receiveHeader(timeout);
	}

	virtual bool negotiateHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
		sendHeader(tcpros, caller_id, topicName, topicTypeName, md5sum, latching);
		auto hdr = receiveHeader(tcpros, timeout);
		for (auto& h : hdr) {
			PLOGV << " - Header[" << h.first << "] = " << h.second ;
		}
		if (hdr["topic"] != topicName) {
			PLOGE << "ROSSubscriberWorker::neotiateHeader(" << caller_id << ", " << topicName << ", " << topicTypeName << ") failed. TopicName is invalid(send=" << topicName << ", recv=" << hdr["topic"] << ")" ;
			return false;
		}
	return true; 
	}

	virtual bool spinOnce() {
	  PLOGV << "ROSSubscriberWorker::spinOnce() called";
	  std::lock_guard<std::mutex> grd(tcpros_mutex_);
	  if(tcpros_ ) {
	    if (!tcpros_->isConnected()) {
	      PLOGE << "ROSSubscriberWorker::spinOnce() failed. Detecting SubscriptionWorker is not connected already." ;
	      tcpros_ = nullptr;
	      return false;
	    }
	    if (tcpros_->isPacketReceived()) {
	      auto maybePacket = tcpros_->receivePacket(receiveTimeout_);
	      if (!maybePacket) {
		PLOGE << "ROSSubscriberWorker::spinOnce() failed. maybePacket is invalid." ;
		return false;
	      }
	      auto maybeMsg = packer_->toMsg(maybePacket);
	      if (maybeMsg) {
		callback_(maybeMsg);
		return true;
	      }
	      else {
		PLOGE << "ROSSubscriberWorker::spinOnce() failed. maybeMsg is nullopt." ;
	      }
	    }
	  } else {
	    PLOGV << "ROSSubscriberWorker::spinOnce: tcpros_ member is nullptr.";
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
		if (packer_) {
			PLOGV << "ROSSubscriberImpl::ROSSubscriberImpl(" << topicName << ", " << packer->typeName() << ") called" ;
		}
		else {
			PLOGV << "ROSSubscriberImpl::ROSSubscriberImpl(" << topicName << ", " << "packer=nullptr" << ") called" ;
		}
	}

private:
	virtual std::shared_ptr<ROSSubscriberWorker> connect(const std::string& host, const int32_t port, bool latching, const double negotiateTimeout) {
		PLOGV << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") called." ;
		if (!packer_) {
			PLOGD << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") called. Current packer is null. Requesting TopicType from Master server." ;
			auto types = this->node_->master()->getTopicTypes(this->node_->name());
			auto topicName = this->getTopicName();
			if (!types) { 
				PLOGE << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") failed. TopicTypes can not be found." ;
				return nullptr;
			}
			auto vs = filter<TopicTypeInfo>(types->topicTypes, [topicName](const TopicTypeInfo& v) { return v.topicName == topicName; });
			if (vs.size() == 0) { 
				PLOGE << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") failed. Appropriate topic(" << topicName << ") not found." ;
     			return nullptr;
			}
			auto topicTypeName = vs[0].topicType;
			auto packer = getROSMsgPackerFactory()->getPacker(topicTypeName);
			if (!packer) {
				PLOGE << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") failed. Packer for topicType(" << topicTypeName << ") can not load" ;
				return nullptr;
			}
			packer_ = packer;
		}
		auto worker = std::make_shared<ROSSubscriberWorker>(packer_, callback_);
		if(worker->connect(host, port, node_->name(), topicName_, packer_->typeName(), packer_->md5sum(), latching, negotiateTimeout)) {
			return std::static_pointer_cast<ROSSubscriberWorker>(worker);
		}
		PLOGE << "ROSSbuscriberImpl::connect(" << host << ", " << port << ") failed. " ;
		return nullptr;
	} 


public:
	virtual bool connect(const std::string& uri, const bool latching=true, const double negotiateTimeout=1.0) override { 
		PLOGV << "ROSSbuscriberImpl::connect(" << uri << ") called." ;
		disconnectUri(uri);
		auto slave = rosslave(uri);
		auto result = slave->requestTopic(node_->name(), topicName_, {ProtocolInfo(std::string("TCPROS"), node_->slaveServer()->getSlaveIP(), getEmptyPort(node_->getPortRange()))});
		if (!result) return false;
		if (result->code != 1) {
			PLOGE << "ROSSbuscriberImpl::connect(" << uri << ") failed. requestTopic method failed." ;
			return false;
		}
		auto worker = connect(result->protocolInfo.arg0, result->protocolInfo.arg1, latching, negotiateTimeout);
		if (!worker) {
			PLOGE << "ROSSbuscriberImpl::connect(" << uri << ") failed. Connecting worker failed." ;
			return false;
		}
		{
			std::lock_guard<std::mutex> grd(workers_mutex_);
			workers_.push_back(worker);
		}
		return true;
	}

	virtual bool disconnectUri(const std::string& uri) override {
		PLOGV << "ROSSbuscriberImpl::disconnectUri(" << uri << ") called." ;
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
	  PLOGV << "ROSSubscriberImpl::spinOnce() called";
		std::lock_guard<std::mutex> grd(workers_mutex_);
		for (auto it = workers_.begin(); it != workers_.end(); ) {
			auto& worker = *it;
			if (!worker->spinOnce()) {
			  PLOGD << "ROSSubscriberImpl::spinOnce() worker failed.";
			  //it = workers_.erase(it);
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
