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
ADD_SUBDIRECTORY(iGPS)
ADD_SUBDIRECTORY(iInsSBG)
ADD_SUBDIRECTORY(pCustomLogger)

ADD_SUBDIRECTORY(lib)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

