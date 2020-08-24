
function(add_sub NAME)
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME})
    add_subdirectory(${NAME})
  endif ()
endfunction(add_sub)

set(nanoros_FOUND "Yes")

add_sub(roscpp)
add_sub(std_msgs)
add_sub(geometry_msgs)
add_sub(sensor_msgs)
add_sub(actionlib_msgs)
add_sub(nav_msgs)


