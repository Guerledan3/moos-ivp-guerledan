##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ./lib ./lib_*/lib ./lib_*/include)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

add_definitions(${CMAKE_CXX_FLAGS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(iSounder)
<<<<<<< HEAD
#ADD_SUBDIRECTORY(iGPS)
#ADD_SUBDIRECTORY(iInsSBG)
#ADD_SUBDIRECTORY(pCustomLogger)
#ADD_SUBDIRECTORY(pLatLon2LocalGrid)
=======
ADD_SUBDIRECTORY(iGPS)
ADD_SUBDIRECTORY(iInsSBG)
ADD_SUBDIRECTORY(pCustomLogger)
>>>>>>> b2535f1d9f6634a86238c984a12ed1349f448020
ADD_SUBDIRECTORY(pLatLongToLamb)
ADD_SUBDIRECTORY(pLogLine)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(lib_behavior_groupe3_guerledan)
ADD_SUBDIRECTORY(pMain)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

