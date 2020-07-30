set(_headers

dllproxy.h
json.h
nanoros.h
nanoros_define.h
os.h
rosmaster.h
rosmsg.h
rosmsgstub.h
rosmsgstubfactory.h
rosnode.h
rosprimitive.h
rospublisher.h
rosrate.h
rosserviceconsumer.h
rosserviceprovider.h
rossrv.h
rossrvstub.h
rossrvstubfactory.h
rossubscriber.h
rostcpros.h
rostcprospacket.h
rosutil.h
signal.h
stringutil.h
)

set(nanoros_hdrs)
foreach(a IN ITEMS ${_headers})
  list(APPEND nanoros_hdrs include/${PROJECT_NAME}/${a})
endforeach()

set(nanoros_headers_ ${nanoros_hdrs} PARENT_SCOPE)
