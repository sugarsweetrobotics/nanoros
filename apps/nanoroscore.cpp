

#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmasterserver.h"

#include <iostream>

using namespace ssr::nanoros;


int main(const int argc, const char* argv[]) {
    ssr::nanoros::init_nanoros(argc, argv);

    std::cout << "nanoroscore:" << std::endl;

    auto info = getROSMasterInfo();
    if (!info) {
        std::cout << "  message: " << "Can not get Master Server URI information." << std::endl;
        return -1;
    }
    std::cout << "  masterServer: " << std::endl;
    std::cout << "    address: " << info->first << std::endl;
    std::cout << "    port: " << info->second << std::endl;
    auto master_server = masterServer(info);
    master_server->start(false);

    master_server->spin();
//    spin();

    return 0;
}