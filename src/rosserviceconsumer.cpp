

#include "nanoros/rosserviceconsumer.h"
#include "nanoros/rosutil.h"
#include "nanoros/rostcpros.h"
#include "nanoros/stringutil.h"


using namespace ssr::nanoros;




class ROSServiceConsumerImpl : public ROSServiceConsumer {
private:
    const std::string& uri_;
    std::string type_;
    std::shared_ptr<TCPROS> tcpros_;
public:
    ROSServiceConsumerImpl(const std::string& caller_id, const std::string& service_name, const std::string& uri): ROSServiceConsumer(caller_id, service_name), uri_(uri), type_("") {
        
    }

    virtual ~ROSServiceConsumerImpl() {}

    bool connect(const double timeout=1.0) {
        auto adr = stringSplit(uri_, '/');
        auto uri = splitUri(adr[2]);
        if (!uri) return false;
        auto& [host, port] = uri.value();
        tcpros_ = tcpros_connect(host, port);
        std::map<std::string, std::string> hdr;
        hdr["callerid"] = callerId();
        hdr["md5sum"] = "*";
        hdr["service"] = serviceName();
        tcpros_->sendHeader(hdr);
        auto rcvHdr = tcpros_->receiveHeader(timeout);

        type_ = rcvHdr["type"];
        return true;
    }

    bool disconnect() {
        return true;
    }
    
public:
    virtual std::optional<std::string> typeName() const override {
        return type_;
    }


    virtual std::shared_ptr<const ROSSrvResult> call(const std::shared_ptr<ROSSrvStub>& stub, const std::shared_ptr<const ROSSrvArg>& arg, const double timeout=1.0) override {
        if (!arg) return nullptr;
        if (!stub) return nullptr;
        if (!tcpros_) return nullptr;
        if (!tcpros_->sendPacket(stub->toPacket(*arg.get()))) return nullptr;
        
        int popedCount = 0;
        auto ok = tcpros_->receiveByte(timeout);
        if (!ok) return nullptr;
        if (ok.value()) {
            return stub->toSrvResult(tcpros_->receivePacket(timeout), popedCount);
        }

        auto msg = tcpros_->receiveString(timeout);
        return std::make_shared<ROSSrvResult>(msg.value());
    }

};




std::shared_ptr<ROSServiceConsumer> ssr::nanoros::serviceConsumer(const std::shared_ptr<ROSMaster>& master, const std::string& caller_id, const std::string& service_name) {
    auto srvInfo = master->lookupService(caller_id, service_name);
    if (!srvInfo) return std::make_shared<ROSServiceConsumer>(caller_id, service_name);
    if (srvInfo->code != 1) return std::make_shared<ROSServiceConsumer>(caller_id, service_name);

    auto srv = std::make_shared<ROSServiceConsumerImpl>(caller_id, service_name, srvInfo->uri);
    if (!srv->connect()) return std::make_shared<ROSServiceConsumer>(caller_id, service_name);
    return srv;
}