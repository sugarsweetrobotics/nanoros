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
  virtual ~ROSNodeImpl() {}
public:
  virtual std::shared_ptr<ROSSlaveServer> slaveServer() override { return slaveServer_; }

  virtual std::shared_ptr<ROSPublisher> getPublisher(const std::string& topicName) const override { 
    for(auto pub: publishers_) {
      if (pub->topicName() == topicName) return pub;
    }  
    return nullptr;
  }
public:
  virtual std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) override {
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

