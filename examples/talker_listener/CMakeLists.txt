

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/basic_stubs)
#include_directories(${PROJECT_SOURCE_DIR}/include)

set(EXT_LIBRARIES nanoros)

function(add_app NAME)
add_executable(${NAME} ${NAME}.cpp)
target_link_libraries(${NAME} ${EXT_LIBRARIES})
set_target_properties(${NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PKGNAME}" 
)
endfunction(add_app)


add_app(talker)
add_app(listener)

