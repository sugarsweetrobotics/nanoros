

include_directories(${CMAKE_SOURCE_DIR}/include)
#include_directories(${PROJECT_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)



add_msg_packer(std_msgs Int32)
add_msg_packer(std_msgs Float32)
add_msg_packer(std_msgs String)
add_msg_packer(std_msgs Header)
add_msg_packer(std_msgs MultiArrayDimension)
add_msg_packer(std_msgs MultiArrayLayout)
add_msg_packer(std_msgs ByteMultiArray)
