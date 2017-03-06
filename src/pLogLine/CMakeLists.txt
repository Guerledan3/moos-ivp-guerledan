#--------------------------------------------------------
# The CMakeLists.txt for:                       pLogLine
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  LogLine.cpp
  LogLine_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pLogLine ${SRC})

TARGET_LINK_LIBRARIES(pLogLine
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

