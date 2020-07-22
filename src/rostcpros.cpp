#include "nanoros/rostcpros.h"
#include "nanoros/stringutil.h"
#include "aqua2/socket.h"
#include "aqua2/serversocket.h"

#include <thread>
#include <chrono>

using namespace ssr::nanoros;

namespace {

}

class TCPROSImpl : public TCPROS {
private:
    ssr::aqua2::ServerSocket serverSocket_;
    ssr::aqua2::Socket socket_;
    bool okay_;
public:
    TCPROSImpl(const std::string& host, const int32_t port): socket_(host.c_str(), port) {
    }


    TCPROSImpl() {
    }

    virtual ~TCPROSImpl() {
        if(socket_.okay()) socket_.close();
        serverSocket_.close();
    }

public:
    virtual bool okay() override { return socket_.okay(); }

    virtual bool close() { return socket_.close(); }

    virtual bool listen(const std::string& host, const int32_t port, const int32_t timeoutUsec = 3000*1000) {
        try {
            serverSocket_.bind(port);
            serverSocket_.listen(5);
            socket_ = serverSocket_.accept();
        } catch (ssr::aqua2::SocketException& ex) {
            return false;
        }
        return true;
    }
public:

    virtual std::optional<TCPROSPacket> receivePacket(const int32_t timeout = -1) override { 
        uint32_t bytes = 0;
        while (socket_.getSizeInRxBuffer() < sizeof(int32_t)) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        socket_.read(&bytes, sizeof(int32_t));
        uint32_t size = from_little_endian(bytes);//ntohl(bytes);
        std::vector<uint8_t> val(size);
        while (socket_.getSizeInRxBuffer() < size) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        socket_.read(&(val[0]), size);
        return TCPROSPacket(std::move(val)); 
    }

    virtual bool sendPacket(const std::shared_ptr<TCPROSPacket>& pkt)  override{ 
        int32_t size = to_little_endian(pkt->bytes().size()); // littel_endian
        //int32_t byte = size; // htonl(size);
        socket_.write(&size, 4);
        socket_.write(&(pkt->bytes()[0]), size);
        //for(int i = 0;i < size;i++) {
        //    uint8_t b = pkt->bytes()[i];
        //    socket_.write(&b, 1);
        //}
        return true;
    }

    virtual bool sendPacket(TCPROSPacket&& pkt) override { 
        int32_t size = to_little_endian(pkt.bytes().size()); // littel_endian must be
        //int32_t byte = size; // htonl(size);
        socket_.write(&size, sizeof(int32_t));
        socket_.write(&(pkt.bytes()[0]), size);
        //for(int i = 0;i < size;i++) {
        //    uint8_t b = pkt.bytes()[i];
        //    socket_.write(&b, 1);
        //}
        return true;
    }
    
    virtual bool sendHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) override {
        TCPROSPacket packet;
        packet.push("callerid="+caller_id);
        packet.push("topic="+topicName);
        packet.push("type="+topicTypeName);
        packet.push("md5sum="+md5sum);
        packet.push("latching="+std::to_string(latching ? 1 : 0));
        return sendPacket(std::move(packet));
    }

    virtual bool sendHeader(const std::map<std::string, std::string>& hdr) override {
        TCPROSPacket packet;
        for(auto&[k, v] : hdr) {
            packet.push(k + "=" + v);
        }
        return sendPacket(std::move(packet));
    }

    virtual std::map<std::string, std::string> receiveHeader(const double timeout)  override {
        std::map<std::string, std::string> hdr;
        auto maybePacket = receivePacket(timeout);
        if (!maybePacket) { return hdr; }

        while(true) {	
            auto tokens = stringSplit(maybePacket->pop<std::string>(), '=');
            if (!tokens) break;
            if (tokens->size() == 2) {
                hdr[tokens.value()[0]] = tokens.value()[1];
            }
        }
        return hdr;
    }

private:

};


std::shared_ptr<TCPROS> ssr::nanoros::tcpros_listen(const std::string& host, const int32_t port) {
    auto tcpros = std::make_shared<TCPROSImpl>();
    tcpros->listen(host, port);
    return tcpros;
}

std::shared_ptr<TCPROS> ssr::nanoros::tcpros_connect(const std::string& host, const int32_t port) {
    return std::static_pointer_cast<TCPROS>(std::make_shared<TCPROSImpl>(host, port));
}

bool ssr::nanoros::tcpros_disconnect(const std::shared_ptr<TCPROS>& tcpros) {
    auto obj = std::static_pointer_cast<TCPROSImpl>(tcpros);
    return obj->close();
}