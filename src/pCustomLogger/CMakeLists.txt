#--------------------------------------------------------
# The CMakeLists.txt for:                       pCustomLogger
# Author(s):                              Anthony Welte
#--------------------------------------------------------

set(CMAKE_CXX_STANDARD 11)

SET(SRC
  CustomLogger.cpp
  CustomLogger_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pCustomLogger ${SRC})

TARGET_LINK_LIBRARIES(pCustomLogger
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

