
#include "nanoros/nanoros.h"
#include "aqua2/socket.h"
#include "nanoros/signal.h"
#include "nanoros/os.h"

#include "nanoros/rosmsgpackerfactory.h"
#include "nanoros/rossrvpackerfactory.h"

#include "nanoros/argparse.h"
#include "nanoros/stringutil.h"

#include "plog/Log.h"
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Initializers/RollingFileInitializer.h"


#include <thread>
#include <filesystem>

namespace {

  volatile bool shutdown_flag = false;

    void signal_handler(const int32_t sig) {
        PLOGV << "SIGNAL(" << sig << ") is captured.";
        shutdown_flag = true;
    }
}

const char* ssr::nanoros::nanoros_version_str() {
    return NANOROS_VERSION_STRING;
}
void ssr::nanoros::init_nanoros(const int argc, const char* argv[]) {
    ssr::nanoros::ArgParser parser;
    // parser.option()


    std::string logfileName("nanoros.log");
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logfileName.c_str());
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

   // plog::Severity
    
    PLOGV << "ssr::nanoros::init_nanoros() called.";

    PLOGV << " - starting nanoros (VERSION=" << NANOROS_VERSION_STRING << ")" ;
#ifdef WIN32
    PLOGV << " - Platform: Windows" ;
#elif linux
    PLOGV << " - Platform: Linux" ;
#else
    PLOGV << " - Platform: Unknown" ;
#endif

    ssr::aqua2::initializeSocket();

    ssr::nanoros::signal(ssr::nanoros::SIGNAL_INT, signal_handler);

    auto absPath = getExecutablePath(argv[0]);
    std::filesystem::path p = absPath;

    PLOGV << " - executable path = " << p ;

    getROSMsgPackerFactory()->addPackerDirHint(p.parent_path().string());
    auto packerDir = ssr::nanoros::getEnv("NANOROS_PACKER_DIR");
    if (packerDir.length() > 0) {
        if (packerDir.rfind('/') != packerDir.length() - 1) {
            packerDir += '/';
        }
        PLOGV << " - NANOROS_PACKER_DIR:" << packerDir ;
        getROSMsgPackerFactory()->addPackerDirHint(packerDir);

    }
    auto packerDirs = ssr::nanoros::getEnv("NANOROS_PACKER_DIRS");
    if (packerDirs.length() > 0) {
    #ifdef WIN32
        const char sep = ';';
    #else
        const char sep = ':';
    #endif
        auto dirs = ssr::nanoros::stringSplit(packerDirs, sep);
        PLOGV << " - NANOROS_PACKER_DIRS:" ;
        for (auto dir : dirs) {
            if (dir.rfind('/') != dir.length() - 1) {
                dir += '/';
            }
            PLOGV << " -  - dir:" << dir << std:: endl;
            getROSMsgPackerFactory()->addPackerDirHint(dir);
        }
    }

    auto hint0 = (p.parent_path().parent_path() / "share" / "nanoros" / "packers").string();
    PLOGV << " - PackerDir Hint: " << hint0 ;
    getROSMsgPackerFactory()->addPackerDirHint(hint0);//absPath + "../share/nanoros/packers/");
#ifdef WIN32
    auto hint1 = absPath + "../../share/nanoros/packers/";
    PLOGV << " - PackerDir Hint: " << hint1 ;
    getROSMsgPackerFactory()->addPackerDirHint(hint1);
#else

#endif
}


void ssr::nanoros::spin() {
    PLOGV << "ssr::nanoros::spin() called" ;
    /// TODO: This must be changed to wait_for method ...
    while(!is_shutdown()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));      
    }
    PLOGV << "ssr::nanoros::spin() exit" ;
}

bool ssr::nanoros::is_shutdown() {
    return shutdown_flag;
}

bool ssr::nanoros::sleep_for(const ssr::nanoros::Duration& duration) {
    uint64_t nanosec = duration.sec * 1.0E+9;
    std::this_thread::sleep_for(std::chrono::nanoseconds(nanosec));
    return true;
}
