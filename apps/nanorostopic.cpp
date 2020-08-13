#include <iostream>
#include <set>
#include <thread>

#include "nanoros/os.h"
#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmaster.h"
#include "nanoros/rosnode.h"
#include "nanoros/rosmsgpackerfactory.h"

using namespace ssr::nanoros;


std::optional<std::set<std::string>> topicNames(const std::optional<SystemState>& state) {
  if (!state) {return std::nullopt;}
  std::set<std::string> topicNameSet;
  for(auto& tpi : state->systemState.publishers) {
    topicNameSet.insert(tpi.topicName);
  }
  for(auto& tsi : state->systemState.subscribers) {
    topicNameSet.insert(tsi.topicName);
  }
  return topicNameSet;
}

template<typename T>
std::vector<T> filter(const std::vector<T>& vals, const std::function<bool(const T&)>& func) {
  std::vector<T> retval;
  for(auto& val : vals) {
    if (func(val)) retval.push_back(val);
  }
  return retval;
}
  

std::string topicType(const std::optional<TopicTypes>& types, const std::string& topicName) {
  if (!types) { return ""; }
  auto vs = filter<TopicTypeInfo>(types->topicTypes, [topicName](const TopicTypeInfo& v) { return v.topicName == topicName; });
  if (vs.size() > 0) { return vs[0].topicType; }
  return "";
}

void show(const std::string& str) {
  std::cout << str << std::endl;
}

template<typename T>
void show(const std::optional<T>& vals) {
  if (!vals) return;
  for(auto& n : vals.value()) {
    show(n);
  }
}

using option_type = std::tuple<std::string, std::string, std::string, std::string, std::function<int32_t(std::string)>>;
// 0. Short option
// 1. Long Option
// 2. Help topic
// 3. Option type. boolean (not need argument), other (need argument)
// 4. callback. Callback must return zero if success.

// Accessor
std::function<std::string(option_type)> shortOpt = [](auto opt) { return std::get<0>(opt); };
std::function<std::string(option_type)> longOpt = [](auto opt) { return std::get<1>(opt); };
std::function<std::string(option_type)> optHelp = [](auto opt) { return std::get<2>(opt); };
std::function<std::string(option_type)> optType = [](auto opt) { return std::get<3>(opt); };
std::function<std::function<int32_t(std::string)>(option_type)> optCB = [](auto opt) { return std::get<4>(opt); };

// Help constructor
void showHelp(const std::string& helpTopic, const std::vector<option_type>& options) {
    std::cout << helpTopic << std::endl;
    std::cout << std::endl << std::endl;
    
    for (auto opt : options) {
        std::cout << "    " << shortOpt(opt) << ", " << longOpt(opt) << ": " << optHelp(opt) << std::endl;
    }
    exit(0);
}


std::vector<std::string> parseArg(const std::vector<option_type>& options, const std::string& helpTopic, const std::vector<std::string>& args) {
    std::vector<std::string> argv_;
    for (auto i = args.begin(); i != args.end(); ++i) {
        if ((*i)[0] != '-') {
            argv_.push_back(*i);
            continue;
        }
        if ((*i) == "-h" || (*i) == "--help") {
            showHelp(helpTopic, options);
        }
        // Test Options is match
        int ret = [&options, &i, &args]() {
            for (auto opt : options) {
                if ((*i) == longOpt(opt) || (*i) == shortOpt(opt)) {
                    if (optType(opt) == "boolean") {
                        return optCB(opt)("");
                    }
                    else {
                        if (++i == args.end()) return -1; // This option needs argment but not supplied
                        return optCB(opt)(*i);
                    }
                }
            }
            return 1; // Unknown Option
        }(); // Immediately call lambda function
        if (ret != 0) { // Invalid option
            showHelp(helpTopic, options);
        }
    }
    return argv_;
}

int main(const int _argc, const char* _argv[]) {
    ssr::nanoros::init_nanoros(_argc, _argv);

    std::vector<std::string> args;
    for (int i = 0; i < _argc; i++) args.push_back(_argv[i]);

    
    if (_argc >= 2) {  
        std::string cmd = _argv[1];
        if (cmd == "list") {
          auto argv = parseArg({}, "nanorostopic list", args); //  No option
          show(topicNames(rosmaster()->getSystemState("/nanorostopic")));
          exit(0);
        } else if (cmd == "type") {
            auto topic = "nanorostopic type $topic_name";
            std::vector<option_type> options{};
            auto argv = parseArg({}, topic, args);
            const std::string topicName = argv[2];
            if (argv.size() < 3) { // Need more info
                showHelp("nanorostopic type $topic_name", options);
            }
            show(topicType(rosmaster()->getTopicTypes("/nanorostopic"), topicName));
            exit(0);
        }
        else if (cmd == "echo") {
            auto topic = "nanorostopic echo $topic_name";
            std::vector<option_type> options{};
            auto argv = parseArg({}, topic, args);
            if (argv.size() < 3) {
                showHelp(topic, options);
            }
            const std::string topicName = argv[2];
            const std::string topicTypeName = topicType(rosmaster()->getTopicTypes("/nanorostopic"), topicName);
            auto packer = getROSMsgPackerFactory()->getPacker(topicTypeName);
            auto node = registerROSNode("/nanorostopic_echo");
            node->subscribe(topicName, packer, [](auto& topic) {
                std::cout << topic->prettyString() << std::endl
                    << "---" << std::endl;
                });
            while (!ssr::nanoros::is_shutdown()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                node->spinOnce();
            }
            return 0;
        }
        else if (cmd == "pub") {
            auto topic = "nanorostopic pub $topic_name $topic_type_name $topic_value";
            double rate = -1;
            std::vector<option_type> options{
                {"-r", "--rate", "double", "Continuously publish topic with rate [hz]", [&rate](std::string arg) {
                    rate = atof(arg.c_str());
                    return 0;
                }}
            };
            auto argv = parseArg(options, topic, args);
            if (argv.size() < 5) {
                showHelp(topic, options);
            }
            const std::string topicName = argv[2];
            const std::string topicTypeName = argv[3];
            const std::string topicDataStr = argv[4];
            auto packer = getROSMsgPackerFactory()->getPacker(topicTypeName);
            if (!packer) {
                std::cout << "ERROR: Topic Type Packer not found(" << topicTypeName << ")" << std::endl;
                return -1;
            }
            auto node = registerROSNode("/nanorostopic_pub");
            auto pub = node->advertise(topicName, packer);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (rate > 0) {
                while (!ssr::nanoros::is_shutdown()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<uint32_t>(1.0/rate*1000000)));
                    pub->publish(packer->fromJSON(fromJSONString(topicDataStr)));
                }
            }
            else {
                pub->publish(packer->fromJSON(fromJSONString(topicDataStr)));
                node->spinOnce();
            }
            return 0;
        }
    }
  

    std::cout << "nanorostopic $command $args..." << std::endl;
    std::cout << std::endl;
    std::cout << "list: List topics" << std::endl;
    std::cout << "type: Show type of topic" << std::endl;
    std::cout << "echo: Echo topic value" << std::endl;
    std::cout << "pub:  Publish Topic value" << std::endl;
    return 0;
}
