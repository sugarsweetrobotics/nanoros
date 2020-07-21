#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace ssr {
    namespace nanoros {

        class TCPROSPacket {
        public:
            TCPROSPacket(): fail_(true), popedCount_(0) {}
            TCPROSPacket(std::vector<uint8_t>&& bytes) : fail_(false), popedCount_(0), bytes_(std::move(bytes)) {}
        private:
            bool fail_;
            std::vector<uint8_t> bytes_;
            int popedCount_;
        public:
            const std::vector<uint8_t>& bytes() const { return bytes_; }
        private:
            void _rewind() { popedCount_ = 0; }
            void _rewind(uint32_t i) { popedCount_ -= i; }
        public:   
            void push(const int32_t val) {
                const uint8_t* b = (const uint8_t*)&val;
                for(int i = 0;i < 4;i++) {
                    ///bytes_.push_back((*(b+(3-i))));
                    bytes_.push_back( (uint8_t)((val >> ((i)*8)) & 0x000000FF) );
                    //bytes_.push_back( (uint8_t)((val >> ((3-i)*8)) & 0x000000FF) );
                    //bytes_.push_back((uint8_t)((val >> (3-i)*8) & 0x000000FF));
                }
            }
            void push(const std::string& str) {
                push((int32_t)str.length());
                for(int i = 0;i < str.length();i++) {
                    bytes_.push_back(str[i]);
                }
                //bytes_.push_back('\n');
            }

        public:

            std::optional<int32_t> popInt32() { return popInt32(this->popedCount_); } 

            std::optional<int32_t> popInt32(int32_t& popedCount) const {
                if (bytes_.size() < popedCount+sizeof(int)) return std::nullopt;
                int32_t buf = 0;
                for(int i = 0;i < 4;i++) {
                    buf |= ((int32_t)(bytes_[popedCount++])) << (8* (i)); 
                }
                //popedCount += 4;
                return buf;
            }

            std::optional<std::string> popString() { return popString(this->popedCount_); } 

            std::optional<std::string> popString(int32_t& popedCount) const {
                auto size = popInt32(popedCount);
                if (!size) {
                    popedCount -= sizeof(int);
                    return std::nullopt;
                }
                if (bytes_.size() < popedCount + size.value()) {
                    popedCount -= sizeof(int);
                    return std::nullopt;
                }
                auto str = std::string((const char*)(&(bytes_[popedCount])), size.value());
                popedCount += size.value();
                return str;
            }

        public:
           operator bool&() { return fail_; }
        };

        class TCPROS {
        public:
            TCPROS() {}
            virtual ~TCPROS() {}

        public:
            virtual bool okay() { return false; }
        public:
            virtual std::optional<TCPROSPacket> receivePacket(const int32_t timeout = -1) { return std::nullopt; }
            virtual bool sendPacket(TCPROSPacket&& pkt) { return false; }
        };

        std::shared_ptr<TCPROS> tcpros_connect(const std::string& host, const int32_t port);
        bool tcpros_disconnect(const std::shared_ptr<TCPROS>& tcpros);
    }
}