


#include "nanoros/rospublisher.h"
#include "nanoros/"
#include <vector>
#include <thread>

#include "aqua2/serversocket.h"

using namespace ssr::nanoros;

class ROSPublisherWorker {
    ssr::aqua2::ServerSocket serverSocket_;
    std::thread thread_;

	std::shared_ptr<TCPROS> tcpros_;
public:
    ROSPublisherWorker& wait(const std::string& caller_id, const std::string& selfIP, const int32_t port) {
        thread_ = std::thread([this, caller_id, selfIP, port]() {
            try {
                serverSocket_.bind(port);
                serverSocket_.listen(5);
                serverSocket_.accept(-1);

                negotiateHeader();

            } catch (ssr::aqua2::SocketException& ex) {
                serverSocket_.close();
            }
        });
        return *this;
    }

    virtual bool negotiateHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {		
        auto hdr = receiveHeader(timeout);
        sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
        if (hdr["topic"] != topicName) return false;
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
};

class ROSPublisherImpl: public ROSPublisher {
    ROSNode* node_;
    const std::shared_ptr<ROSMsgStub> stub_;
    std::vector<std::shared_ptr<ROSPublisherWorker>> workers_;
public:
    ROSPublisherImpl(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub) :  ROSPublisher(topicName), node_(node), stub_(stub) {

    }
    virtual ~ROSPublisherImpl() {}

public:
    virtual bool publish(const std::shared_ptr<const ROSMsg>& msg) override {

    }

    virtual bool standBy(const std::string& caller_id, const std::string& selfIP, const int32_t port) override  {
        auto worker = std::make_shared<ROSPublisherWorker>();
        worker->wait(caller_id, selfIP, port);
        workers_.emplace_back(std::move(worker));
        return true;
    }
};

std::shared_ptr<ROSPublisher> ssr::nanoros::createROSPublisher(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub) {
    return std::make_shared<ROSPublisherImpl>(node, topicName, stub);
}