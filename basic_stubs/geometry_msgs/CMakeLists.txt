

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)

function(add_msg_stub PKGNAME NAME)
add_library(${NAME} SHARED ${NAME}.cpp)
target_link_libraries(${NAME} ${EXT_LIBRARIES})
set_target_properties(${NAME} PROPERTIES PREFIX "")
set_target_properties(${NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/msg"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/msg"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/msg" 
)
endfunction(add_msg_stub)


add_msg_stub(std_msgs Twist)
add_msg_stub(std_msgs Vector3)