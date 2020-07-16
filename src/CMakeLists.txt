set(_srcs

rosmaster.cpp

)



include_directories(${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME})


foreach(a IN ITEMS ${nanoros_headers})
  list(APPEND nanoros_hdrs ${CMAKE_SOURCE_DIR}/${a})
endforeach()

add_library(${PROJECT_NAME} SHARED ${_srcs} ${nanoros_hdrs})
set_target_properties(${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

