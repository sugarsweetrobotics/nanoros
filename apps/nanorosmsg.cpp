
#include "nanoros/nanoros.h"

#include "nanoros/rosmsgpackerfactory.h"
#include "nanoros/stringutil.h"
#include "argparser.h"

#include <fstream>
#include <filesystem>

using namespace ssr::nanoros;

int main(const int _argc, const char* _argv[]) {
    ssr::nanoros::init_nanoros(_argc, _argv);

    std::vector<std::string> args;
    for (int i = 0; i < _argc; i++) args.push_back(_argv[i]);

    
    if (_argc >= 2) {  
        std::string cmd = _argv[1];
        if (cmd == "show") {
            auto topic = "nanorosmsg show $topic_type_name";
            std::vector<ssr::option_type> options{}; //  No option

            auto argv = ssr::parseArg({}, topic, args);
            if (argv.size() < 3) { // If no argument
                ssr::showHelp(topic, options);
            }
            

            auto fullTypeName = args[2];

            auto packer = getROSMsgPackerFactory()->getPacker(fullTypeName);
            if (!packer) {
                std::cout << " Can not find type information of " << fullTypeName << std::endl;
                return -1;
            }
            std::cout << packer->typeInfo() << std::endl;

            return 0;
            
        }
        else if (cmd == "md5") {
            auto topic = "nanorosmsg md5 $topic_type_name";
            std::vector<ssr::option_type> options{}; //  No option

            auto argv = ssr::parseArg({}, topic, args);
            if (argv.size() < 3) { // If no argument
                ssr::showHelp(topic, options);
            }

            auto fullTypeName = args[2];
            auto packer = getROSMsgPackerFactory()->getPacker(fullTypeName);
            if (!packer) {
                std::cout << " Can not find type information of " << fullTypeName << std::endl;
                return -1;
            }
            std::cout << packer->md5sum() << std::endl;
            return 0;
        }


    }
    return 0;
}