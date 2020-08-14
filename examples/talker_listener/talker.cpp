#include <iostream>
#include <set>
#include <thread>


#include "nanoros/nanoros.h"
#include "nanoros/rosnode.h"
#include "std_msgs/msg/String.h"


int main(const int argc, const char* argv[]) {
    ssr::nanoros::init_nanoros(argc, argv);
    std::cout << "Starting nanoros talker" << std::endl;


    auto node = ssr::nanoros::registerROSNode("/talker");
    auto pub =  node->advertise<ssr::nanoros::std_msgs::String>("/hello");
    if (!pub) {
        std::cout << "Publisher advertise failed." << std::endl;
        return -1;
    }
    
    ssr::nanoros::std_msgs::String value;
    value.data = "Hello";

    const ssr::nanoros::Duration duration(1.0);
    int i = 0;
    while(!ssr::nanoros::is_shutdown()) {

        value.data = "Hello (" + std::to_string(i) + ")";
        std::cout << "talker [" << i << "]: " << value.data << std::endl;
        pub->publish(value);
        node->spinOnce();
        ssr::nanoros::sleep_for(duration);
        i++;
    }
    std::cout << "shutting down talker" << std::endl;
    return 0;
}