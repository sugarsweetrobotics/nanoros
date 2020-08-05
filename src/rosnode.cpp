#include "nanoros/os.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosnode.h"
#include "nanoros/rosslave.h"
#include "nanoros/rosslaveserver.h"
#include "nanoros/rossubscriber.h"
#include "nanoros/rosserviceprovider.h"

#include <thread>

using namespace ssr::nanoros;


class ROSNodeImpl : public ROSNode {
private:
  const std::shared_ptr<ROSMaster> master_;
  const std::shared_ptr<ROSSlaveServer> slaveServer_;

  const int32_t portBase_ = 30000;

  std::vector<std::shared_ptr<ROSSubscriber>> subscribers_;
  std::vector<std::shared_ptr<ROSPublisher>> publishers_;
  std::vector<std::shared_ptr<ROSServiceProvider>> serviceProviders_;
public:
  ROSNodeImpl(const std::shared_ptr<ROSMaster>& master, const std::string& name) : ROSNode(name), master_(master), slaveServer_(rosslaveserver(this, master, getSelfIP(), getEmptyPort(getPortRange()))) {}
  virtual ~ROSNodeImpl() {
    unsubscribeAll();
    unadvertiseAll();
    unadvertiseServiceAll();
  }
public:



  virtual std::shared_ptr<ROSSlaveServer> slaveServer() override { return slaveServer_; }

  virtual std::shared_ptr<ROSPublisher> getRegisteredPublisher(const std::string& topicName) const override { 
    for(auto pub: publishers_) {
      if (pub->getTopicName() == topicName) return pub;
    }  
    return nullptr;
  }
public:
  virtual std::shared_ptr<ROSSubscriber> getRegisteredSubscriber(const std::string& topicName) const override {
    for(auto& sub : subscribers_) {
      if (sub->getTopicName() == topicName) return sub;
    }
    return nullptr;
  }

  virtual std::shared_ptr<ROSServiceProvider> getRegisteredServiceProvider(const std::string& srvName) const override {
    for(auto& sp : serviceProviders_) {
      if (sp->getServiceName() == srvName) return sp;
    }
    return nullptr;
  }
public:
  virtual std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) override {
    if (getRegisteredSubscriber(topicName) != nullptr) return nullptr;
    auto subscriber = createROSSubscriber(this, topicName, packer, func);
    if (!subscriber) return nullptr;
    auto publishersInfo = master_->registerSubscriber(name_, topicName, packer->typeName(), slaveServer_->getSlaveUri());
    if (publishersInfo->code) {
      for(auto& pub: publishersInfo->publishers) {
        subscriber->connect(pub, latching, negotiateTimeout);
      }
    }
    subscribers_.push_back(subscriber);
    return subscriber;
  }

  virtual bool subscribeUri(const std::string& topicName, const std::string& uri, const bool latching=false, const double negotiateTimeout=1.0) override { 
    auto subscriber = this->getRegisteredSubscriber(topicName);
    if (!subscriber) return false;
    subscriber->connect(uri, latching, negotiateTimeout);
    return false; 
  }

  virtual bool unsubscribeUri(const std::string& topicName, const std::string& uri) override { 
    auto subscriber = this->getRegisteredSubscriber(topicName);
    if (!subscriber) return false;
    return subscriber->disconnectUri(uri);
  }


  virtual bool unsubscribe(const std::string& topicName) override {
    return unsubscribe(getRegisteredSubscriber(topicName));
  }

  virtual bool unsubscribe(const std::shared_ptr<ROSSubscriber>& sub) {
    if (!sub) return false;
    if (!sub->disconnect()) return false;
    auto result = master_->unregisterSubscriber(name_, sub->getTopicName(), sub->getTopicTypeName(), slaveServer_->getSlaveUri());
    if (getRegisteredSubscriber(sub->getTopicName()) != nullptr) {
      for(auto it = subscribers_.begin(); it != subscribers_.end(); ++it) {
        if ((*it)->getTopicName() == sub->getTopicName()) {
          it = subscribers_.erase(it);
          return true;
        } 
      }
    }
    return true;
  }

  virtual void unsubscribeAll() override {
    for(auto& sub: subscribers_) {
      if (!unsubscribe(sub)) {}
    }
  }

  virtual void unadvertiseServiceAll() override {
    for(auto& sub: serviceProviders_) {
      if (!unadvertiseService(sub)) {}
    }
  }

  virtual std::shared_ptr<ROSPublisher> advertise(const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer, const double negotiateTimeout=1.0) override {
    if (getRegisteredPublisher(topicName) != nullptr) return nullptr;
    auto publisher = createROSPublisher(this, topicName, packer);
    if (!publisher) return nullptr;
    auto subscribersInfo = master_->registerPublisher(name_, topicName, packer->typeName(), slaveServer_->getSlaveUri());
    if (subscribersInfo->code) {
      for(auto& sub: subscribersInfo->subscribers) {
        // do nothing
      }
    }
    publishers_.push_back(publisher);
    return publisher;
  }

  virtual bool advertiseService(const std::string& srvName, const std::shared_ptr<ROSSrvPacker>& packer, const std::function<const std::shared_ptr<ROSSrvResponse>(const std::shared_ptr<const ROSSrvRequest>&)>& func) override {
    if (getRegisteredServiceProvider(srvName) != nullptr) return false;
    auto provider = createROSServiceProvider(this, srvName, packer, func);
    if (!provider) return false;
    auto subscribersInfo = master_->registerService(name_, srvName, provider->getUri(), slaveServer_->getSlaveUri());
    serviceProviders_.push_back(provider);
    return true; 
  }


  virtual bool unadvertise(const std::string& topicName) override {
    return unadvertise(getRegisteredPublisher(topicName));
  }

  virtual bool unadvertise(const std::shared_ptr<ROSPublisher>& pub) override {
    if (!pub) return false;
    auto result = master_->unregisterPublisher(name_, pub->getTopicName(), pub->getTopicTypeName(), slaveServer_->getSlaveUri());

    if (getRegisteredPublisher(pub->getTopicName()) != nullptr) {
      for(auto it = publishers_.begin(); it != publishers_.end(); ++it) {
        if ((*it)->getTopicName() == pub->getTopicName()) {
          it = publishers_.erase(it);
          return true;
        } 
      }
    }
    return true;
  }
  
  virtual bool unadvertiseService(const std::shared_ptr<ROSServiceProvider> & provider) override {
    if (!provider) return false;
    auto subscribersInfo = master_->unregisterService(name_, provider->getServiceName(), provider->getUri(), slaveServer_->getSlaveUri());

    if (getRegisteredServiceProvider(provider->getServiceName()) != nullptr) {
      for(auto it = serviceProviders_.begin(); it != serviceProviders_.end(); ++it) {
        if ((*it)->getServiceName() == provider->getServiceName()) {
          it = serviceProviders_.erase(it);
          return true;
        } 
      }
    }

    return false;
  }


  virtual void unadvertiseAll() override {
    for(auto& pub: publishers_) {
      if (!unadvertise(pub)) {}
    }
  }

  virtual void spinOnce() override {
    for(auto& sub: subscribers_) {
      sub->spinOnce();
    }
  }

  virtual void spin() override  {
    while(true) {
      spinOnce();
      //std::wait
    }
  }

  virtual void spin(const Rate& rate) override {
    while(true) {
      spinOnce();
      std::this_thread::sleep_for(std::chrono::microseconds(static_cast<uint64_t>(rate.getIntervalUsec())));
    }
  }

  virtual std::optional<std::vector<TopicTypeInfo>> getPublications() const override { 
    std::vector<TopicTypeInfo> val;
    for(auto pub: publishers_) {
      val.emplace_back(TopicTypeInfo(pub->getTopicName(), pub->getTopicTypeName()));
    }
    return val;
  }

  virtual std::optional<std::vector<TopicTypeInfo>> getSubscriptions() const override { 
    std::vector<TopicTypeInfo> val;
    for(auto sub: subscribers_) {
      val.emplace_back(TopicTypeInfo(sub->getTopicName(), sub->getTopicTypeName()));
    }
    return val;
  }

  virtual std::optional<std::vector<std::string>> getCurrentSubscribingPublisherUris() const override { 
    std::vector<std::string> val;
    for(auto sub: subscribers_) {
      auto opt= sub->getSubscribingPublisherUris();
      if (!opt) return std::nullopt;
      for(auto pubUri : opt.value()) {
        val.push_back(pubUri);
      }
    }
    return val;
  }


};


std::shared_ptr<ROSNode> ssr::nanoros::registerROSNode(const std::shared_ptr<ROSMaster>& master, const std::string& nodeName) {
  return std::static_pointer_cast<ROSNode>(std::make_shared<ROSNodeImpl>(master, nodeName));
}

