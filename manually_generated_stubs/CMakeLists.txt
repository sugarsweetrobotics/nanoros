
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set(EXT_LIBRARIES nanoros)

set(STUB_BASE_DIR share/nanoros/stubs)

function(add_msg_stub PKGNAME NAME)
  add_library(${PKGNAME}_${NAME} SHARED msg/${NAME}.h msg/${NAME}.cpp)
  target_link_libraries(${PKGNAME}_${NAME} ${EXT_LIBRARIES})
  set_target_properties(${PKGNAME}_${NAME} 
    PROPERTIES 
    PREFIX ""
    OUTPUT_NAME ${NAME}
  )
  set_target_properties(${PKGNAME}_${NAME}
    PROPERTIES PUBLIC_HEADER "msg/${NAME}.h"
  )
  set_target_properties(${PKGNAME}_${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/msg"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/msg"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/msg" 
  )
  install(TARGETS ${PKGNAME}_${NAME}
    RUNTIME DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/msg"
    LIBRARY DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/msg"
    ARCHIVE DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/msg"
    PUBLIC_HEADER DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/msg"
  )
endfunction(add_msg_stub)


function(add_srv_stub PKGNAME NAME)
  add_library(${PKGNAME}_${NAME} SHARED srv/${NAME}.h srv/${NAME}.cpp)
  target_link_libraries(${PKGNAME}_${NAME} ${EXT_LIBRARIES})
  set_target_properties(${PKGNAME}_${NAME} 
    PROPERTIES 
    PREFIX ""
    OUTPUT_NAME ${NAME}
  )
  set_target_properties(${PKGNAME}_${NAME}
    PROPERTIES PUBLIC_HEADER "srv/${NAME}.h"
  )
  set_target_properties(${PKGNAME}_${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/srv"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/srv"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${STUB_BASE_DIR}/${PKGNAME}/srv" 
  )
  install(TARGETS ${PKGNAME}_${NAME}
    RUNTIME DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/srv"
    LIBRARY DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/srv"
    ARCHIVE DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/srv"
    PUBLIC_HEADER DESTINATION "${STUB_BASE_DIR}/${PKGNAME}/srv"
  )
endfunction(add_srv_stub)

add_subdirectory(roscpp)
add_subdirectory(std_msgs)
add_subdirectory(geometry_msgs)
add_subdirectory(beginner_tutorials)

set(STUB_INSTALL_DIR ${STUB_BASE_DIR} PARENT_SCOPE)