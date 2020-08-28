set(_srcs

nanoros.cpp
rosmaster.cpp
rosutil.cpp
os.cpp
signal.cpp
dllproxy.cpp
json.cpp
packerfactory.cpp
rosmsg.cpp
rosmsgpacker.cpp
rosmsgpackerfactory.cpp
rosnode.cpp
rosslave.cpp
rosslaveserver.cpp
rossubscriber.cpp
rospublisher.cpp
rostcpros.cpp
rosserviceconsumer.cpp
rossrvpacker.cpp
rossrvpackerfactory.cpp
rosserviceprovider.cpp

rosmasterserver.cpp
)


set(EXT_LIBRARIES XmlRpc)

include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/XmlRpc/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/nlohmann-json/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/libaqua2/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/plog/include)


foreach(a IN ITEMS ${nanoros_headers})
  list(APPEND nanoros_hdrs ${CMAKE_SOURCE_DIR}/${a})
endforeach()


if (WIN32)
set(CMAKE_DEBUG_POSTFIX d)
endif (WIN32)

add_library(${PROJECT_NAME} SHARED ${_srcs} ${nanoros_hdrs} ${nanoros_public_hdrs})
target_link_libraries(${PROJECT_NAME} ${EXT_LIBRARIES})			    
set_target_properties(${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if (WIN32)
INSTALL(FILE ${CMAKE_BINARY_DIR}/bin/Debug/${PROJECT_NAME}d.dll 
  DESTINATION "${CMAKE_BINARY_DIR}/bin")
endif (WIN32)

