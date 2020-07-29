#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <map>

#include "nanoros/rosmsg.h"
#include "nanoros/rostcprospacket.h"

namespace ssr {
    namespace nanoros {



        class TCPROS {
        public:
            TCPROS() {}
            virtual ~TCPROS() {}

        public:
        
            virtual bool okay() { return false; }

            virtual bool isConnected() { return false; }
        public:

            virtual std::optional<uint8_t> receiveByte(const int32_t timeout = -1) { return std::nullopt; }


            virtual std::optional<std::string> receiveString(const int32_t timeout = -1) { return std::nullopt; }

            virtual std::optional<TCPROSPacket> receivePacket(const int32_t timeout = -1) { return std::nullopt; }

            virtual bool sendPacket(const std::shared_ptr<TCPROSPacket>& pkt) {return false; }
            virtual bool sendPacket(TCPROSPacket&& pkt) { return false; }


            virtual bool sendHeader(const std::string& caller_id, const std::string& topicName, const std::string& topicTypeName, const std::string& md5sum, const bool latching) {
                return false;
            }
            virtual bool sendHeader(const std::map<std::string, std::string>& hdr) {
                return false;
            }
            virtual std::map<std::string, std::string> receiveHeader(const double timeout) { return {}; }


        };

        std::shared_ptr<TCPROS> tcpros_connect(const std::string& host, const int32_t port);
        std::shared_ptr<TCPROS> tcpros_listen(const std::string& host, const int32_t port);
        bool tcpros_disconnect(const std::shared_ptr<TCPROS>& tcpros);
    }
}