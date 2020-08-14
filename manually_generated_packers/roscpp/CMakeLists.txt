

include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)

add_msg_packer(roscpp Logger)
add_srv_packer(roscpp SetLoggerLevel)
add_srv_packer(roscpp GetLoggers)
