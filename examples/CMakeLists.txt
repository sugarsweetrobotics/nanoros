
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../automatically_generated_packers/std_msgs)
add_subdirectory(talker_listener)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../automatically_generated_packers/beginner_tutorials)
add_subdirectory(beginner_tutorials)
endif()
