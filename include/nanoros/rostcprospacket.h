#pragma once


#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <optional>

namespace ssr {
    namespace nanoros {

        
        template<typename T>
        T to_little_endian(T&& t) { 
#ifdef HOST_BIG_ENDIAN

#else 
            return t; 
#endif
        }

        template<typename T>
        T from_little_endian(T&& t) { 
#ifdef HOST_BIG_ENDIAN

#else 
            return t; 
#endif
        }

        union double_uint64 {
            double doubleValue;
            uint64_t uint64Value;
        };

        union float_uint32 {
            float floatValue;
            uint32_t uint32Value;
        };


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
            template<typename T>
            void push(const T& val) {
                const uint8_t* b = (const uint8_t*)&val;
                for(size_t i = 0;i < sizeof(T);i++) {
#ifdef HOST_BIG_ENDIAN
                    bytes_.push_back( (uint8_t)((val >> ((sizeof(T)-1-i)*8)) & 0x000000FF) );
#else
                    bytes_.push_back( (uint8_t)((val >> ((i)*8)) & 0x000000FF) );
#endif
                }
            }

            template<typename T>
            void pushVector(const std::vector<T>& val) {
                const uint32_t size = val.size();
                push<uint32_t>(size);
                for (auto& e : val) {
                    push(e);
                }
            }

            template<>
            void push<std::string>(const std::string& str) {
                push<uint32_t>(str.length());
                for(int i = 0;i < str.length();i++) {
                    bytes_.push_back(str[i]);
                }
            }

            template<>
            void push<double>(const double& data) {
                double_uint64 val;
                val.doubleValue = data;
                push<uint64_t>(val.uint64Value);
            }

            template<>
            void push<float>(const float& data) {
                float_uint32 val;
                val.floatValue = data;
                push<uint32_t>(val.uint32Value);
            }

            template<>
            void push<ssr::nanoros::time>(const ssr::nanoros::time& t) {
                push<uint32_t>(t.sec);
                push<uint32_t>(t.nsec);
            }

            template<>
            void push<ssr::nanoros::TCPROSPacket>(const ssr::nanoros::TCPROSPacket& t) {
                auto size = t.bytes().size();
                for(size_t i = 0;i < size;i++) {
                    push<uint8_t>(t.bytes()[i]);
                }
            }

        public:
            template<typename T>
            std::optional<T> pop() {
                return pop<T>(this->popedCount_);
            }


            template<typename T>
            std::optional<T> pop(int32_t& popedCount) const {
                if (bytes_.size() < popedCount+sizeof(T)) return std::nullopt;
                T buf = 0;
                for(size_t i = 0;i < sizeof(T);i++) {
#ifdef HOST_BIG_ENDIAN
                    buf |= ((T)(bytes_[popedCount++])) << (8* (sizeof(T)-i-1)); 
#else
                    buf |= ((T)(bytes_[popedCount++])) << (8* (i)); 
#endif
                }
                return buf;
            }

            template<typename T>
            std::optional<std::vector<T>> popVector(int32_t& popedCount) const {
                std::vector<T> retval;
                auto size = pop<int32_t>(popedCount);
                if (!size) {
                    return std::nullopt;
                }
                if (bytes_.size() < popedCount + size.value()) {
                    popedCount -= sizeof(int32_t);
                    return std::nullopt;
                }
                for (size_t i = 0; i < size; i++) {
                    auto buf = pop<T>(popedCount);
                    if (!buf) {
                        popedCount -= sizeof(T) * i;
                        return std::nullopt;
                    }
                    retval.push_back(buf.value());
                }
                return retval;
            }

            std::optional<int32_t> popInt32() { return popInt32(this->popedCount_); } 

            std::optional<int32_t> popInt32(int32_t& popedCount) const {
                if (bytes_.size() < popedCount+sizeof(int32_t)) return std::nullopt;
                int32_t buf = 0;
                for(int i = 0;i < 4;i++) {
                    buf |= ((int32_t)(bytes_[popedCount++])) << (8* (i)); 
                }
                //popedCount += 4;
                return buf;
            }

            std::optional<uint32_t> popUint32() { return popUint32(this->popedCount_); } 

            std::optional<uint32_t> popUint32(int32_t& popedCount) const {
                if (bytes_.size() < popedCount+sizeof(uint32_t)) return std::nullopt;
                uint32_t buf = 0;
                for(int i = 0;i < 4;i++) {
                    buf |= ((uint32_t)(bytes_[popedCount++])) << (8* (i)); 
                }
                return buf;
            }

            std::optional<std::string> popString() { return popString(this->popedCount_); } 

            template<>
            std::optional<std::string> pop<std::string>(int32_t& popedCount) const {
                auto size = pop<int32_t>(popedCount);
                if (!size) {
                    //popedCount -= sizeof(int32_t);
                    return std::nullopt;
                }
                if (bytes_.size() < popedCount + size.value()) {
                    popedCount -= sizeof(int32_t);
                    return std::nullopt;
                }
                auto str = std::string((const char*)(&(bytes_[popedCount])), size.value());
                popedCount += size.value();
                return str;
            }

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

            template<>
            std::optional<ssr::nanoros::time> pop<ssr::nanoros::time>(int32_t& popedCount) const {
                ssr::nanoros::time t;
                auto arg0 = pop<uint32_t>(popedCount);
                if (!arg0) return std::nullopt;
                auto arg1 = pop<uint32_t>(popedCount);
                if (!arg1) return std::nullopt;
                return ssr::nanoros::time{arg0.value(), arg1.value()};
            }

            std::optional<ssr::nanoros::time> popTime(int32_t& popedCount) const {
                ssr::nanoros::time t;
                auto arg0 = popUint32(popedCount);
                if (!arg0) return std::nullopt;
                auto arg1 = popUint32(popedCount);
                if (!arg1) return std::nullopt;
                return ssr::nanoros::time{arg0.value(), arg1.value()};
            }

            template<>
            std::optional<double> pop<double>(int32_t& popedCount) const {
                double_uint64 val;
                auto temp = pop<uint64_t>(popedCount);
                if (!temp) {
                    return std::nullopt;
                }
                val.uint64Value = temp.value();
                return val.doubleValue;
            }

            template<>
            std::optional<float> pop<float>(int32_t& popedCount) const {
                float_uint32 val;
                auto temp = pop<uint32_t>(popedCount);
                if (!temp) {
                    return std::nullopt;
                }
                val.uint32Value = temp.value();
                return val.floatValue;
            }

        public:
           operator bool&() { return fail_; }


        public:
        };
    }
}