include_directories(${CMAKE_SOURCE_DIR}/include/)

set(EXT_LIBRARIES nanoros)

set(APPS )

function(add_app NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} ${EXT_LIBRARIES})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
  )
  set(APPS ${APPS} ${NAME} PARENT_SCOPE)
endfunction(add_app)


add_app(nanorostopic)
add_app(nanorosservice)

message("APPS IS " ${APPS})

install(TARGETS ${APPS})