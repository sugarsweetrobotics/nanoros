
#include "aqua2/serversocket.h"


#include "nanoros/rospublisher.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rosmsgpacker.h"
#include <vector>
#include <thread>
#include <mutex>


using namespace ssr::nanoros;

class ROSPublisherWorker {
    //ssr::aqua2::ServerSocket serverSocket_;
    std::shared_ptr<std::thread> thread_;

	std::shared_ptr<TCPROS> tcpros_;
    bool connected_;
    std::string caller_id_;
    std::string selfIP_;
    int32_t port_;
    std::string topicName_;
    std::string topicTypeName_;
    std::string md5sum_;

    std::mutex tcpros_mutex_;
public:
    ROSPublisherWorker(): connected_(false), port_(-1) {}

    virtual ~ROSPublisherWorker() {
        if (thread_) {
            if (!connected_) {
                ssr::aqua2::Socket socket;
                socket.connect("localhost", port_);
                socket.close();
            }
            thread_->join();
        }
    }
public:

    bool isConnected() { 
        std::lock_guard<std::mutex> grd(tcpros_mutex_);
        return tcpros_->isConnected();
    }

    ROSPublisherWorker& wait(const std::string& caller_id, const std::string& selfIP, const int32_t port, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
        caller_id_ = caller_id;
        selfIP_ = selfIP;
        port_ = port;
        topicName_ = topicName;
        topicTypeName_ = topicTypeName;
        md5sum_ = md5sum;
        thread_ = std::make_shared<std::thread>([this, latching, timeout]() {
            try {
                auto tcpros = tcpros_server();
                tcpros->bind("0.0.0.0", port_);
                tcpros->listen(5);
                tcpros->accept();
                negotiateHeader(tcpros, caller_id_, topicName_, topicTypeName_, md5sum_, latching, timeout);
                connected_ = true;
                {
                    std::lock_guard<std::mutex> grd(tcpros_mutex_);
                    tcpros_ = tcpros;
                }
            } catch (ssr::aqua2::SocketException& ex) {
                //tcpros_->close();
                //serverSocket_.close();
            } catch (std::exception& ex) {
                //tcpros_->close();
                //serverSocket_.close();
            }
        });
        return *this;
    }

    virtual bool negotiateHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching, const double timeout=1.0) {
        auto hdr = receiveHeader(tcpros, timeout);
        if (!sendHeader(tcpros, caller_id, topicName, topicTypeName, md5sum, latching)) return false;
        if (hdr["topic"] != topicName) return false;
        return true; 
    }

    virtual bool sendHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
        std::lock_guard<std::mutex> grd(tcpros_mutex_);
        return sendHeader(tcpros_, caller_id, topicName, topicTypeName, md5sum, latching);
    }

    virtual bool sendHeader(std::shared_ptr<TCPROS>& tcpros, const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
        if (!tcpros) return false;
        return tcpros->sendHeader(caller_id, topicName, topicTypeName, md5sum, latching);
    }

    virtual std::map<std::string, std::string> receiveHeader(const double timeout) {
        std::lock_guard<std::mutex> grd(tcpros_mutex_);
        return receiveHeader(tcpros_, timeout);
    }

    virtual std::map<std::string, std::string> receiveHeader(std::shared_ptr<TCPROS>& tcpros, const double timeout) {
        if (!tcpros) return {};
        return tcpros->receiveHeader(timeout);
    }

    virtual void sendPacket(const std::shared_ptr<TCPROSPacket>& pkt) {
        std::lock_guard<std::mutex> grd(tcpros_mutex_);
        if (tcpros_) {
            tcpros_->sendPacket(pkt);
        }
    }
};

class ROSPublisherImpl: public ROSPublisher {
private:

    const std::shared_ptr<ROSMsgPacker> packer_;
    std::mutex workers_mutex_;
    std::vector<std::shared_ptr<ROSPublisherWorker>> workers_;

public:
    ROSPublisherImpl(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer) :  
        ROSPublisher(node, topicName), packer_(packer) {
    }
    virtual ~ROSPublisherImpl() {}

public:

    virtual std::string getTopicTypeName() const override { return packer_->typeName(); }

public:

    virtual bool isConnected() override { 
        bool flag = false;
        std::lock_guard<std::mutex> grd(workers_mutex_);
        for(auto& worker : workers_) {
            flag |= worker->isConnected();
        }
        return flag;
    }

    virtual bool publish(const ROSMsg& msg) override {
        try {
            auto pkt = packer_->toPacket(msg);
            if (pkt) {
                std::lock_guard<std::mutex> grd(workers_mutex_);
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
        worker->wait(caller_id, selfIP, port, getTopicName(), packer_->typeName(), packer_->md5sum(), false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::lock_guard<std::mutex> grd(workers_mutex_);
            workers_.emplace_back(std::move(worker));
        }
        return true;
    }
};

std::shared_ptr<ROSPublisher> ssr::nanoros::createROSPublisher(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer) {
    if (!packer) return nullptr;
    return std::make_shared<ROSPublisherImpl>(node, topicName, packer);
}