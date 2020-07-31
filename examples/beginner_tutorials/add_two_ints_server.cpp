#include <iostream>
#include <set>
#include <thread>


#include "nanoros/nanoros.h"
#include "nanoros/rosnode.h"
#include "beginner_tutorials/AddTwoInts.h"


int main(const int argc, const char* argv[]) {
    ssr::nanoros::init_nanoros(argc, argv);


    auto node = ssr::nanoros::registerROSNode("/add_two_ints_server");
    
    auto pub =  node->advertiseService<ssr::nanoros::beginner_tutorials::AddTwoIntsRequest, ssr::nanoros::beginner_tutorials::AddTwoIntsResponse>("/add_two_ints", 
    [](const std::shared_ptr<const ssr::nanoros::beginner_tutorials::AddTwoIntsRequest>& req) -> const std::shared_ptr<ssr::nanoros::beginner_tutorials::AddTwoIntsResponse> {
        auto res = std::make_shared<ssr::nanoros::beginner_tutorials::AddTwoIntsResponse>();
        res->sum = req->a + req->b;
        return res;
    });
        

    const ssr::nanoros::Duration duration(1.0);
    int i = 0;
    while(!ssr::nanoros::is_shutdown()) {
        node->spinOnce();
        ssr::nanoros::sleep_for(duration);
    }
    return 0;
}