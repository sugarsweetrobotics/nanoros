#include <iostream>
#include <set>
#include <thread>


#include "nanoros/nanoros.h"
#include "nanoros/rosnode.h"
#include "std_msgs/String.h"


int main(const int argc, const char* argv[]) {
    ssr::nanoros::init_nanoros();


    auto node = ssr::nanoros::registerROSNode("/talker");
    auto pub =  node->advertise<ssr::nanoros::std_msgs::String>("/hello");
    
    ssr::nanoros::std_msgs::String value;
    value.data = "Hello";

    const ssr::nanoros::Duration duration(1.0);
    while(!ssr::nanoros::is_shutdown()) {
        std::cout << "talker: " << value.data << std::endl;
        pub->publish(value);
        node->spinOnce();
        ssr::nanoros::sleep_for(duration);
    }
    std::cout << "shutting down talker" << std::endl;
    return 0;
}