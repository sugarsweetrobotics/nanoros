
set(_public_headers
argparse.h

nanoros.h
nanoros_define.h

rosmaster.h
rosnode.h

rosprimitive.h

rosmsg.h
rosmsgpacker.h
rosmsgpackerfactory.h

rossrv.h
rossrvpacker.h
rossrvpackerfactory.h

rospublisher.h
rossubscriber.h

rostcpros.h 
rostcprospacket.h 
rosrate.h
rosutil.h
json.h
packerfactory.h
dllproxy.h

rosslave.h
rosserviceprovider.h 
rosserviceconsumer.h 


)

set(_headers
argparse.h
dllproxy.h
json.h
nanoros.h
nanoros_define.h
os.h
rosmaster.h
rosmsg.h
rosmsgpacker.h
rosmsgpackerfactory.h
rosnode.h
rosprimitive.h
rospublisher.h
rosrate.h
rosserviceconsumer.h
rosserviceprovider.h
rossrv.h
rossrvpacker.h
rossrvpackerfactory.h
rossubscriber.h
rostcpros.h
rostcprospacket.h
rosutil.h
signal.h
stringutil.h
packerfactory.h

rosmasterserver.h
)

set(nanoros_hdrs)

foreach(a IN ITEMS ${_headers})
  list(APPEND nanoros_hdrs include/${PROJECT_NAME}/${a})
endforeach()

set(nanoros_public_hdrs)
foreach(a IN ITEMS ${_public_headers})
  list(APPEND nanoros_public_hdrs include/${PROJECT_NAME}/${a})
endforeach()

set(nanoros_headers_ ${nanoros_hdrs} PARENT_SCOPE)
set(nanoros_public_headers_ ${nanoros_public_hdrs} PARENT_SCOPE)
