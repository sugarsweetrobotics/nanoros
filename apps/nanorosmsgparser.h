#pragma once

#include <string>
#include "msgparser/msgparser.h"

const std::string hdr_part[] = {
	R"(
#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>
)",
R"(
namespace ssr::nanoros {
)"

};

const std::string src_part[] = {
	R"(
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
)"

};

const std::string msgcmk[] = {
    R"(
function(add_msg_packer PKGNAME NAME)
  add_library(${PKGNAME}_${NAME} SHARED ${NAME}.h ${NAME}.cpp)
  target_link_libraries(${PKGNAME}_${NAME} ${EXT_LIBRARIES})
  set_target_properties(${PKGNAME}_${NAME} 
    PROPERTIES 
    PREFIX ""
    OUTPUT_NAME ${NAME}
  )
  set_target_properties(${PKGNAME}_${NAME}
    PROPERTIES PUBLIC_HEADER "${NAME}.h"
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
  install(FILES ${NAME}.msg
    DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/msg"
  )
endfunction(add_msg_packer)

)"
};

const std::string srvcmk[] = {
    R"(

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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PACKER_BASE_DIR}/${PKGNAME}/srv" 
  )
  install(TARGETS ${PKGNAME}_${NAME}
    RUNTIME DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    LIBRARY DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    ARCHIVE DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
    PUBLIC_HEADER DESTINATION "${PACKER_BASE_DIR}/${PKGNAME}/srv"
  )
endfunction(add_srv_packer)


)"
};

const std::string cmk[] = {
    R"(
cmake_minimum_required(VERSION 3.5)


set(VENDOR_NAME "nanoros_vendor_id")
set(PROJECT_DESCRIPTION "Msg Packer for nanoros")
set(PROJECT_VERSION "0.0.1")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_SUBMINOR 1)
)",

R"(
set(CMAKE_USE_RELATIVE_PATHS True)
set(CMAKE_CXX_STANDARD 17)

if (!${nanoros_FOUND})
find_package(nanoros REQUIRED)
endif ()
)",
//find_package(nanoros REQUIRED)
R"(

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${nanoros_INCLUDE_DIR})

link_directories(${nanoros_LIB_DIR})

set(EXT_LIBRARIES nanoros)

set(PACKER_BASE_DIR share/nanoros/packers)
set(LIB_INSTALL_DIR lib)


add_subdirectory(msg)
#add_subdirectory(srv)


### Generating Installer Code

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
  PATH_VARS PACKER_BASE_DIR)

)",


R"(
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${NERIKIRI_VERSION}
        COMPATIBILITY SameMajorVersion )
        
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake 
)


set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "${VENDOR_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_SUBMINOR})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
SET(CPACK_NSIS_MODIFY_PATH ON)
)",


R"(

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")

INCLUDE(CPack)



)"
};


const std::string cmake_in[] = {
    R"()"


};