#--------------------------------------------------------
# The CMakeLists.txt for:                       iInsSBG
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  InsSBG.cpp
  InsSBG_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iInsSBG ${SRC})

TARGET_LINK_LIBRARIES(iInsSBG
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   sbgECom
   pthread)

