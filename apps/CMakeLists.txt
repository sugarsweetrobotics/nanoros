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
add_app(nanoroscore)

# add_app(nanorosmsgparser)
set(md5_codes 
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/hashlibpp/src/hl_md5.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/hashlibpp/src/hl_md5wrapper.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/hashlibpp/src/hl_md5.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/hashlibpp/src/hl_md5wrapper.cpp 
)
  include_directories(../thirdparty/hashlibpp/src)
  add_executable(nanorosmsgparser nanorosmsgparser.cpp msgparser/msgparser.h msgparser/msgparser.cpp msgparser/md5.cpp ${md5_codes})
  target_link_libraries(nanorosmsgparser ${EXT_LIBRARIES})
  set_target_properties(nanorosmsgparser
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
  )
  set(APPS ${APPS} nanorosmsgparser PARENT_SCOPE)


install(TARGETS ${APPS})