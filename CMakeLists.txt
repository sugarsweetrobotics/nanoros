cmake_minimum_required(VERSION 3.5)


set(NANOROS_VERSION 0.0.1)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_SUBMINOR 1)

project(nanoros LANGUAGES CXX VERSION ${NANOROS_VERSION})
set(CMAKE_USE_RELATIVE_PATHS True)
set(CMAKE_CXX_STANDARD 17)

option(nanoros_JSON_Install Off)


set(nanoros_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PACKER_BASE_DIR share/nanoros/packers)
set(PACKER_INSTALL_DIR ${PACKER_BASE_DIR} )


add_subdirectory(thirdparty)

add_compile_definitions("NANOROS_VERSION_STRING=\"${PROJECT_VERSION}\"")

add_subdirectory(include)
add_subdirectorY(src)

add_subdirectory(apps)
add_subdirectory(examples)
add_subdirectory(manually_generated_packers)
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/automatically_generated_packers)
add_subdirectory(automatically_generated_packers)
endif ()

set(INCLUDE_INSTALL_DIR include/ )
set(RUNTIME_INSTALL_DIR bin )
set(LIB_INSTALL_DIR lib/ )
set(SHARE_INSTALL_DIR share/nanoros )
set(SYSCONFIG_INSTALL_DIR etc/${PROJECT_NAME}/ )

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${nanoros_public_headers}")

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
  PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR PACKER_INSTALL_DIR LIB_INSTALL_DIR RUNTIME_INSTALL_DIR SYSCONFIG_INSTALL_DIR)

configure_file(cmake/cpack_wix_patch_path.patch.in
  ${CMAKE_CURRENT_BINARY_DIR}/cpack_wix_patch_path.patch
  #PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR PACKER_INSTALL_DIR LIB_INSTALL_DIR RUNTIME_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)


write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${NERIKIRI_VERSION}
        COMPATIBILITY SameMajorVersion )
        
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake 
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CREDIT.txt 
  DESTINATION ${SHARE_INSTALL_DIR}
)

INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})




set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "SUGAR SWEET ROBOTICS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "nanoros - Nano-size Robot Operating System implementation")
set(CPACK_PACKAGE_VERSION ${NANOROS_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_SUBMINOR})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nanoros")
SET(CPACK_NSIS_MODIFY_PATH ON)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WixID.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/WixID.cmake)
endif()

# set(CPACK_WIX_UPGRADE_GUID "")
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/cpack_wix_patch_path.patch")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")

INCLUDE(CPack)
