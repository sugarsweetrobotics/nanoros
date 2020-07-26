


#include "nanoros/rospublisher.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rosmsgstub.h"
#include <vector>
#include <thread>

#include "aqua2/serversocket.h"

using namespace ssr::nanoros;

class ROSPublisherWorker {
    ssr::aqua2::ServerSocket serverSocket_;
    std::thread thread_;

	std::shared_ptr<TCPROS> tcpros_;
public:
    ROSPublisherWorker& wait(const std::string& caller_id, const std::string& selfIP, const int32_t port, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
        thread_ = std::thread([this, caller_id, selfIP, port, topicName, topicTypeName, md5sum, latching, timeout]() {
            try {
                tcpros_ = tcpros_listen("0.0.0.0", port);
                negotiateHeader(caller_id, topicName, topicTypeName, md5sum, latching, timeout);
            } catch (ssr::aqua2::SocketException& ex) {
                serverSocket_.close();
            } catch (std::exception& ex) {
                serverSocket_.close();
            }
        });
        return *this;
    }

    virtual bool negotiateHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {		
        auto hdr = receiveHeader(timeout);
        if (!sendHeader(caller_id, topicName, topicTypeName, md5sum, latching)) return false;
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

    virtual void sendPacket(const std::shared_ptr<TCPROSPacket>& pkt) {
        if (tcpros_) {
            tcpros_->sendPacket(pkt);
        }
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
    virtual bool publish(const ROSMsg& msg) override {
        try {
            auto pkt = stub_->toPacket(msg);
            if (pkt) {
                for(auto& worker : workers_) {
                    worker->sendPacket(pkt);
                }
            }
            return true;
        } catch (std::bad_cast& bc) {
            std::cerr << "ROSPublisherImpl::publish() catched exception: Bad Cast" << std::endl;
            return false;
        }
    }


    virtual bool publish(const std::shared_ptr<ROSMsg>& msg) override {
        if (!msg) return false;
        return publish(*(msg.get()));
    }

    virtual bool standBy(const std::string& caller_id, const std::string& selfIP, const int32_t port) override  {
        auto worker = std::make_shared<ROSPublisherWorker>();
        worker->wait(caller_id, selfIP, port, topicName(), stub_->typeName(), stub_->md5sum(), false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        workers_.emplace_back(std::move(worker));
        return true;
    }
};

std::shared_ptr<ROSPublisher> ssr::nanoros::createROSPublisher(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub) {
    return std::make_shared<ROSPublisherImpl>(node, topicName, stub);
}