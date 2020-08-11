
#include "aqua2/socket.h"


#include "nanoros/rosserviceprovider.h"

#include "nanoros/rosnode.h"
#include "nanoros/rostcpros.h"

#include <iostream>
#include <thread>

using namespace ssr::nanoros;

class ROSServiceProviderImpl : public ROSServiceProvider {

private:
    int32_t port_;
    const std::shared_ptr<ROSSrvPacker> packer_;
    const std::function<const std::shared_ptr<ROSMsg>(const std::shared_ptr<const ROSMsg>&)> callback_;
    std::shared_ptr<std::thread> thread_;
    std::shared_ptr<TCPROS> tcpros_;
public:
    ROSServiceProviderImpl(ROSNode* node, const std::string& serviceName, const std::shared_ptr<ROSSrvPacker>& packer, const std::function<const std::shared_ptr<ROSMsg>(const std::shared_ptr<const ROSMsg>&)>& func) :
        ROSServiceProvider(node, serviceName), packer_(packer), callback_(func) {
        port_ = getEmptyPort(40000);

        thread_ = std::make_shared<std::thread>([this]() {
            std::map<std::string, std::string> hdr;

            hdr["callerid"] = node_->name();
            hdr["md5sum"] = packer_->md5sum();
            hdr["service"] = getServiceName();
            hdr["type"] = packer_->typeName();
            hdr["response_type"] = packer_->typeName() + "Response";
            hdr["request_type"] = packer_->typeName() + "Request";

            tcpros_ = tcpros_server();
            tcpros_->bind("0.0.0.0", port_);
            tcpros_->listen(5);

            while(true) {
                tcpros_->accept();
                auto recvHeader = tcpros_->receiveHeader(1.0);
                tcpros_->sendHeader(hdr);
                if (!checkHeader(hdr)) {
                    tcpros_->disconnect();
                    continue;
                }
                
                if (recvHeader.count("probe") > 0) {
                    tcpros_->disconnect();
                    continue;
                }

                try {
                    while(true) {
                        while(true) {
                            if (tcpros_->rxBufferSize() >= 4) break;
                            if (tcpros_->isConnected() == false) break;
                        }
                        if (tcpros_->isConnected() == false) break;

                        auto pkt = tcpros_->receivePacket();
                        auto req = packer_->toSrvRequest(pkt);
                        if (!req) {
                            tcpros_->sendByte(0);
                            tcpros_->sendString("Invalid Request Message");
                            continue;
                        }
                        auto res = callback_(req);
                        if (!res) {
                            tcpros_->sendByte(0);
                            tcpros_->sendString("Provider's callback can not create Response");
                            continue;
                        }
                        auto respkt = packer_->responseToPacket(*res.get());
                        if (!respkt) {
                            tcpros_->sendByte(0);
                            tcpros_->sendString("Provider's callback created invalid Response");
                            continue;
                        }
                        tcpros_->sendByte(1);
                        tcpros_->sendPacket(respkt);
                    }
                } catch (std::exception& ex) {
                    std::cout << "Service Provider callback made Exception: " << ex.what() << std::endl;
                }
                tcpros_->disconnect();
            }
        });
    }

    bool checkHeader(const std::map<std::string, std::string>& hdr) {
        return true;
    }

    virtual ~ROSServiceProviderImpl() {
        if (thread_) {
            ssr::aqua2::Socket socket;
            socket.connect("localhost", port_);
            socket.close();
            
            thread_->join();
        }
    }

    virtual std::string getUri() const override { 
        return "rosrpc://" + getSelfIP() + ":" + std::to_string(port_); 
    }


};

std::shared_ptr<ROSServiceProvider> ssr::nanoros::createROSServiceProvider(ROSNode* node, const std::string& serviceName, const std::shared_ptr<ROSSrvPacker>& packer,
            const std::function<const std::shared_ptr<ROSMsg>(const std::shared_ptr<const ROSMsg>&)>& func)
            {

	 if (!packer) return nullptr;
	return std::make_shared<ROSServiceProviderImpl>(node, serviceName, packer, func);
}
