#--------------------------------------------------------
# The CMakeLists.txt for:                       pMain
# Author(s):                              Guerlédan 3
#--------------------------------------------------------

SET(SRC
  Main.cpp
  Main_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMain ${SRC})

TARGET_LINK_LIBRARIES(pMain
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

