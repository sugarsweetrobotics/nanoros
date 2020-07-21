#include "nanoros/rostcpros.h"

#include "aqua2/socket.h"


using namespace ssr::nanoros;


class TCPROSImpl : public TCPROS {
private:
    ssr::aqua2::Socket socket_;
    bool okay_;
public:
    TCPROSImpl(const std::string& host, const int32_t port): socket_(host.c_str(), port) {
    }

    virtual ~TCPROSImpl() {
        if(socket_.okay()) socket_.close();
    }

public:
    virtual bool okay() { return socket_.okay(); }

    virtual bool close() { return socket_.close(); }
public:

    virtual std::optional<TCPROSPacket> receivePacket(const int32_t timeout = -1) { 
        uint32_t bytes = 0;
        while (socket_.getSizeInRxBuffer() < 4) {
            //std::cout << "hoge" << std::endl;
        }
        
        socket_.read(&bytes, 4);
        uint32_t size = bytes;//ntohl(bytes);
        std::vector<uint8_t> val(size);
        while (socket_.getSizeInRxBuffer() < size);
            socket_.read(&(val[0]), size);
        return TCPROSPacket(std::move(val)); 
    }


    virtual bool sendPacket(TCPROSPacket&& pkt) { 
        int32_t size = pkt.bytes().size();
        int32_t byte = size; // htonl(size);
        socket_.write(&byte, 4);
        for(int i = 0;i < size;i++) {
            uint8_t b = pkt.bytes()[i];
            socket_.write(&b, 1);
        }
        //uint8_t b = '\n';
        //socket_.write(&b, 1);
        //socket_.write(&(pkt.bytes()), size);
        return true;
    }

private:

};



std::shared_ptr<TCPROS> ssr::nanoros::tcpros_connect(const std::string& host, const int32_t port) {
    return std::static_pointer_cast<TCPROS>(std::make_shared<TCPROSImpl>(host, port));
}

bool ssr::nanoros::tcpros_disconnect(const std::shared_ptr<TCPROS>& tcpros) {
    auto obj = std::static_pointer_cast<TCPROSImpl>(tcpros);
    return obj->close();
}