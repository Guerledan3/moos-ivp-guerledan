#--------------------------------------------------------
# The CMakeLists.txt for:                       pLatLongToLamb
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  LatLongToLamb.cpp
  LatLongToLamb_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pLatLongToLamb ${SRC})

TARGET_LINK_LIBRARIES(pLatLongToLamb
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   proj)

