

include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)

add_msg_stub(roscpp Logger)
add_srv_stub(roscpp SetLoggerLevel)
add_srv_stub(roscpp GetLoggers)
