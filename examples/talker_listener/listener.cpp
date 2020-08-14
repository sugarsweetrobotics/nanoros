#include <iostream>
#include <set>
#include <thread>


#include "nanoros/nanoros.h"
#include "nanoros/rosnode.h"
#include "std_msgs/msg/String.h"


int main(const int argc, const char* argv[]) {
    ssr::nanoros::init_nanoros(argc, argv);

    const ssr::nanoros::Duration duration(1.0);

    auto node = ssr::nanoros::registerROSNode("/listener");
    node->subscribe<ssr::nanoros::std_msgs::String>("/hello", [](const std::shared_ptr<const ssr::nanoros::std_msgs::String>& msg) {
        std::cout << "listner: " << msg->data << std::endl;
    });
    
    while(!ssr::nanoros::is_shutdown()) {
        ssr::nanoros::sleep_for(duration);
        node->spinOnce();
    }

    return 0;
}