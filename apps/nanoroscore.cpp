

#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmasterserver.h"


using namespace ssr::nanoros;


int main(const int argc, const char* argv[]) {

    
    ssr::nanoros::init_nanoros(argc, argv);


    auto master_server = masterServer(getROSMasterInfo());
    master_server->start(false);

    spin();

    return 0;
}