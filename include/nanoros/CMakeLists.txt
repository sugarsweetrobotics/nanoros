set(_headers

rosmaster.h

)

set(nanoros_hdrs)
foreach(a IN ITEMS ${_headers})
  list(APPEND nanoros_hdrs include/${PROJECT_NAME}/${a})
endforeach()

set(nanoros_headers_ ${nanoros_hdrs} PARENT_SCOPE)
