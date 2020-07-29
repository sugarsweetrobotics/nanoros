
include_directories(${CMAKE_CURRENT_SOURCE_DIR})



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


function(add_srv_stub PKGNAME NAME)
add_library(${NAME} SHARED ${NAME}.cpp)
target_link_libraries(${NAME} ${EXT_LIBRARIES})
set_target_properties(${NAME} PROPERTIES PREFIX "")
set_target_properties(${NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/srv"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/srv"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}/srv" 
)
endfunction(add_srv_stub)


add_subdirectory(roscpp)

add_subdirectory(std_msgs)

add_subdirectory(geometry_msgs)


add_subdirectory(beginner_tutorials)