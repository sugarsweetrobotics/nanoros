#include "aqua2/socket.h"
#include "aqua2/serversocket.h"


#include "nanoros/rostcpros.h"
#include "nanoros/stringutil.h"

#include <thread>
#include <chrono>


#include "plog/Log.h"

using namespace ssr::nanoros;

namespace {

}

class TCPROSImpl : public TCPROS {
private:
    ssr::aqua2::ServerSocket serverSocket_;
    ssr::aqua2::Socket socket_;
    bool okay_;

  uint64_t numSentPacket_;
  uint64_t numRecvPacket_;
public:
  TCPROSImpl(const std::string& host, const int32_t port): socket_(host.c_str(), port), numSentPacket_(0), numRecvPacket_(0) {
    }


  TCPROSImpl(): numSentPacket_(0), numRecvPacket_(0) {
    }

    virtual ~TCPROSImpl() {
        if(socket_.okay()) socket_.close();
        serverSocket_.close();
    }

public:
    virtual bool okay() override { return socket_.okay(); }

    virtual bool isConnected() override { return socket_.isConnected(); }

    virtual bool close() { return socket_.close(); }

    virtual bool bind(const std::string& host, const int32_t port, const int32_t timeoutUsec = 3000*1000) override {
        try {
            serverSocket_.bind(port);
        } catch (ssr::aqua2::SocketException& ex) {
            return false;
        }
        return true;
    }

    virtual bool listen(int backlog) override {
        try {
            serverSocket_.listen(backlog);
        } catch (ssr::aqua2::SocketException& ex) {
            return false;
        }
        return true;
    }

    virtual bool accept() override {
        try {
            socket_ = serverSocket_.accept();
        } catch (ssr::aqua2::SocketException& ex) {
            return false;
        }
        return true;
    }


    virtual bool disconnect() override { 
        socket_.close();
        return true; 
    }
public:

    virtual bool sendByte(const int8_t data) override { 
        if (socket_.write(&data, 1) != 1) return false;
        return true; 
    }

    virtual std::optional<uint8_t> receiveByte(const int32_t timeout = -1) override {
        uint8_t bytes = 0;
        while (socket_.getSizeInRxBuffer() < sizeof(int8_t)) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        socket_.read(&bytes, sizeof(uint8_t));
        return bytes;
    }

    virtual bool sendString(const std::string& d) override {
        uint32_t size = d.length();
        if (socket_.write(&size, 4) != sizeof(uint32_t)) return false;
        if (socket_.write(d.c_str(), size) != size) return false;
        return true; 
    }

    virtual std::optional<std::string> receiveString(const int32_t timeout = -1) override {
        uint32_t bytes = 0;
        while (socket_.getSizeInRxBuffer() < sizeof(int32_t)) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        socket_.read(&bytes, sizeof(int32_t));
        uint32_t size = from_little_endian(bytes);//ntohl(bytes);
        char *buf = new char[size+1];
        while (socket_.getSizeInRxBuffer() < size) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        socket_.read(buf, size);
        buf[size] = 0;
        return std::string(buf);
    }

    virtual int32_t rxBufferSize() override {
        return socket_.getSizeInRxBuffer() ;
    }

  virtual bool isPacketReceived() override {
    return socket_.getSizeInRxBuffer() >= sizeof(int32_t);
  }
    
  
    virtual std::optional<TCPROSPacket> receivePacket(const int32_t timeoutMs = -1) override {
      PLOGV << "TCPROSImpl::receivePacket(timeout=" << timeoutMs << ")";
        uint32_t bytes = 0;
        auto start = std::chrono::system_clock::now();
        while (socket_.getSizeInRxBuffer() < sizeof(int32_t)) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            if (timeoutMs >= 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() >= timeoutMs) {
	      PLOGD << "TCPROSImpl::receivePacket timoeut in packet size search case." << std::endl;
                return std::nullopt;
            }
        }
        socket_.read(&bytes, sizeof(int32_t));
        uint32_t size = from_little_endian(bytes);//ntohl(bytes);
	PLOGD << "TCPROSImpl::receivePacket. Receiving packet size is " << size;
	numRecvPacket_++;
        std::vector<uint8_t> val(size);
        while (socket_.getSizeInRxBuffer() < size) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            //if (timeoutMs >= 0 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() >= timeoutMs) {
	    //PLOGD << "TCPROSImpl::receivePacket timoeut." << std::endl;
            //    return std::nullopt;
            //}
        }
        socket_.read(&(val[0]), size);
        return TCPROSPacket(std::move(val)); 
    }

    virtual bool sendPacket(const std::shared_ptr<TCPROSPacket>& pkt)  override{
      PLOGV << "TCPROSImpl::sendPacket(size=" << pkt->bytes().size() << ") called";
        int32_t size = to_little_endian(pkt->bytes().size()); // littel_endian
        if (socket_.write(&size, 4) != 4) {
	  PLOGE << "TCPROSImpl::sendPacket fail. Sending Size index failed.";
	  return false;
	}
        if (socket_.write(&(pkt->bytes()[0]), size) != size) {
	  PLOGE << "TCPROSImpl::sendPacket fail. Sending Packet body failed.";
	  return false;
	}
	numSentPacket_++;
	PLOGD << "TCPROSImpl::sendPacket(num=" << numSentPacket_ << ") success.";		
        return true;
    }

    virtual bool sendPacket(TCPROSPacket&& pkt) override {
      PLOGV << "TCPROSImpl::sendPacket(size=" << pkt.bytes().size() << ") called";      
        int32_t size = to_little_endian(pkt.bytes().size()); // littel_endian must be
        if (socket_.write(&size, sizeof(int32_t)) != sizeof(int32_t)) {
	  PLOGE << "TCPROSImpl::sendPacket fail. Sending Size index failed.";
	  return false;
	}
        if (socket_.write(&(pkt.bytes()[0]), size) != size) {
	  PLOGE << "TCPROSImpl::sendPacket fail. Sending Packet body failed.";
	  return false;
	}
	numSentPacket_++;
	PLOGD << "TCPROSImpl::sendPacket(num=" << numSentPacket_ << ") success.";	
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

    virtual std::map<std::string, std::string> receiveHeader(const double timeoutSec)  override {
        std::map<std::string, std::string> hdr;
        auto maybePacket = receivePacket(timeoutSec*1000);
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


std::shared_ptr<TCPROS> ssr::nanoros::tcpros_server() {
    auto tcpros = std::make_shared<TCPROSImpl>();
    return tcpros;
}

std::shared_ptr<TCPROS> ssr::nanoros::tcpros_connect(const std::string& host, const int32_t port) {
    try {
        auto tcpros = std::make_shared<TCPROSImpl>(host, port);
        return tcpros;
    }
    catch (ssr::aqua2::SocketException& ex) {
        std::cout << "Socket::connect failed. " << ex.what() << std::endl;
        return nullptr;
    }
}

bool ssr::nanoros::tcpros_disconnect(const std::shared_ptr<TCPROS>& tcpros) {
    auto obj = std::static_pointer_cast<TCPROSImpl>(tcpros);
    return obj->close();
}
