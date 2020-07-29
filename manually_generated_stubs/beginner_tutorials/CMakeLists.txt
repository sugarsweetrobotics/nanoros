

include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)

# add_msg_stub(roscpp Logger)
add_srv_stub(beginner_tutorials AddTwoInts)
