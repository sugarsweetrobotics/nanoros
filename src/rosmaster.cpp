#include "nanoros/rosmaster.h"





using namespace ssr::nanoros;



class ROSMasterImpl : public ROSMaster {


public:
  ROSMasterImpl(const std::string& addr, const int32_t port) {}


  virtual ~ROSMasterImpl() {}



public:
  
  virtual std::optional<std::vector<std::string>> getTopicNameList() {
    return std::nullopt;
  }

};



NANOROS_API std::shared_ptr<ROSMaster> ssr::nanoros::rosmaster(const std::string& addr, const int32_t port) {

  return std::static_pointer_cast<ROSMaster>(std::make_shared<ROSMasterImpl>(addr, port));
}
