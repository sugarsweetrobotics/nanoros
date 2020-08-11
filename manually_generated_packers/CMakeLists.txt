
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set(EXT_LIBRARIES nanoros)

set(PACKER_BASE_DIR share/nanoros/packers)

function(add_msg_packer PKGNAME NAME)
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
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/msg"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/msg"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/msg" 
  )
  install(TARGETS ${PKGNAME}_${NAME}
    RUNTIME DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/msg"
    LIBRARY DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/msg"
    ARCHIVE DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/msg"
    PUBLIC_HEADER DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/msg"
  )
endfunction(add_msg_packer)


function(add_srv_packer PKGNAME NAME)
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
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/srv"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/srv"
    RUNTIME_OUTPUT_DIRECTORY "${PACKER_BASE_DIR}/${PKGNAME}/srv" 
  )
  install(TARGETS ${PKGNAME}_${NAME}
    RUNTIME DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    LIBRARY DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    ARCHIVE DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    PUBLIC_HEADER DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
  )
endfunction(add_srv_packer)

# add_subdirectory(roscpp)
# add_subdirectory(std_msgs)
# add_subdirectory(geometry_msgs)
add_subdirectory(beginner_tutorials)

set(PACKER_INSTALL_DIR ${PACKER_BASE_DIR} PARENT_SCOPE)