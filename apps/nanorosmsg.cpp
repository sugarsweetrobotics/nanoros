
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
            auto topic = "nanorosmsg show";
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
            /**

            auto tokens = stringSplit(fullTypeName, '/');
            if (tokens.size() != 2) {
                std::cout << "Error. Invalid TypeName. ex: std_msgs/Int32." << std::endl;
                ssr::showHelp(topic, options);
            }
            auto pkgName = tokens[0];
            auto typeName = tokens[1];
            for (auto dirHint : getROSMsgPackerFactory()->getPackerDirHints()) {
                std::filesystem::path path = dirHint;
                auto filepath = path / pkgName / "msg" / (typeName + ".msg");
                std::cout << " - " << filepath << std::endl;
                if (std::filesystem::is_regular_file(filepath)) {
                    std::ifstream fin(filepath);
                    std::string line;
                    while (std::getline(fin, line)) {
                        std::cout << line << std::endl;
                    }
                    return 0;
                }
#ifdef WIN32
                else {
                    filepath = path / pkgName / "msg" / "Debug" / (typeName + ".msg");
                    std::cout << " - " << filepath << std::endl;
                    if (std::filesystem::is_regular_file(filepath)) {
                        std::ifstream fin(filepath);
                        std::string line;
                        while (std::getline(fin, line)) {
                            std::cout << line << std::endl;
                        }
                        return 0;
                    }
                }
#endif
            }
            
            std::cout << "Do not found type: " << fullTypeName << std::endl;
            exit(1);
            */
        }


    }
    return 0;
}