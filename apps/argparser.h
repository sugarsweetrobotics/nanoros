#pragma once
#include <tuple>
#include <string>
#include <functional>
#include <cstdint>
#include <optional>
#include <iostream>
#include <vector>

namespace ssr {


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


}