#include "nanoros/os.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosnode.h"
#include "nanoros/rosslave.h"
#include "nanoros/rosslaveserver.h"
#include "nanoros/rossubscriber.h"

#include <thread>

using namespace ssr::nanoros;


class ROSNodeImpl : public ROSNode {
private:
  const std::shared_ptr<ROSMaster> master_;
  const std::shared_ptr<ROSSlaveServer> slaveServer_;

  const int32_t portBase_ = 30000;

  std::vector<std::shared_ptr<ROSSubscriber>> subscribers_;
  std::vector<std::shared_ptr<ROSPublisher>> publishers_;
public:
  ROSNodeImpl(const std::shared_ptr<ROSMaster>& master, const std::string& name) : ROSNode(name), master_(master), slaveServer_(rosslaveserver(this, master, getSelfIP(), getEmptyPort(getPortRange()))) {}
  virtual ~ROSNodeImpl() {
    unsubscribeAll();
    unadvertiseAll();
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
public:
  virtual std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) override {
    if (getRegisteredSubscriber(topicName) != nullptr) return nullptr;
    auto subscriber = createROSSubscriber(this, topicName, stub, func);
    auto publishersInfo = master_->registerSubscriber(name_, topicName, stub->typeName(), slaveServer_->getSlaveUri());
    if (publishersInfo->code) {
      for(auto& pub: publishersInfo->publishers) {
        subscriber->connect(pub, latching, negotiateTimeout);
      }
    }
    subscribers_.push_back(subscriber);
    return subscriber;
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

  virtual std::shared_ptr<ROSPublisher> advertise(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const double negotiateTimeout=1.0) override {
    auto publisher = createROSPublisher(this, topicName, stub);
    auto subscribersInfo = master_->registerPublisher(name_, topicName, stub->typeName(), slaveServer_->getSlaveUri());
    if (subscribersInfo->code) {
      for(auto& sub: subscribersInfo->subscribers) {
        // do nothing
      }
    }
    publishers_.push_back(publisher);
    return publisher;
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

};


std::shared_ptr<ROSNode> ssr::nanoros::registerROSNode(const std::shared_ptr<ROSMaster>& master, const std::string& nodeName) {
  return std::static_pointer_cast<ROSNode>(std::make_shared<ROSNodeImpl>(master, nodeName));
}

