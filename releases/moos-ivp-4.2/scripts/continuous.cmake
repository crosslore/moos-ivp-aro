##############################################################################
# AUTHOR:      Christopher Gagner
# DATE:        November 1, 2010
# FILE:        continuous.cmake
# DESCRIPTION: Performs a continuous build for the moos-ivp project. This will
#              checkout the moos-ivp repository to the moos-ivp-continuous
#              directory, update if needed, build, test, and submit the 
#              results to the CDash server.
##############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY( SET CMP0011 NEW )

INCLUDE( build_functions.cmake )

SET( REPO_NAME "moos-ivp" )
SET( REPO_LOCATION "https://oceanai.mit.edu/svn/moos-ivp-aro/trunk" )
SET( WORKING_DIR "./${REPO_NAME}-continuous" )


SET( CTEST_PROJECT_NAME "moos-ivp" )

#-----------------------------------------------------------------------------
# Setup Commands
#-----------------------------------------------------------------------------
FIND_PROGRAM(CMAKE_CMD "cmake")
FIND_PROGRAM(CTEST_CMD "ctest")
FIND_PROGRAM(SVN_CMD "svn")

SET ( CTEST_COMMAND  "${CTEST_CMD} -D Continuous " )
SET ( CTEST_CMAKE_COMMAND "${CMAKE_CMD} " )

IF( ${WIN32} )
   # For Windows, we need to explicitly set the build type to Debug
   SET ( CTEST_COMMAND  "${CTEST_COMMAND} -C Debug " )
   SET ( CTEST_CMAKE_COMMAND "${CTEST_CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug " )
ENDIF( ${WIN32} )

SET( CTEST_SVN_COMMAND ${SVN_CMD} )
SET( CTEST_SVN_CHECKOUT "${CTEST_SVN_COMMAND} co ${REPO_LOCATION} ${WORKING_DIR}" )


SET( CTEST_UPDATE_COMMAND "${CTEST_SVN_COMMAND}" )

#-----------------------------------------------------------------------------
# Checkout the repository and build
#-----------------------------------------------------------------------------

# Check if the working directory exists
IF( NOT EXISTS "${WORKING_DIR}" )
   # Perform a clean checkout
   SET( CTEST_CHECKOUT_COMMAND "${CTEST_SVN_CHECKOUT}")
   SVN_CLEAN_CHECKOUT("${REPO_LOCATION}" "${WORKING_DIR}")
ENDIF( NOT EXISTS "${WORKING_DIR}" )


# Resolve the MOOS symbolic link
RESOLVE_SYMLINK(MOOS_DIR "${WORKING_DIR}/MOOS")

# Build MOOS
BUILD_MOOS(${MOOS_DIR} MOOS_BUILD_OUTPUT)
MESSAGE( ${MOOS_BUILD_OUTPUT} )

# Setup the SOURCE and BUILD directories of IVP
GET_FILENAME_COMPONENT( IVP_SOURCE_DIR "${WORKING_DIR}/ivp/src" ABSOLUTE)
GET_FILENAME_COMPONENT( IVP_BUILD_DIR "${WORKING_DIR}/build" ABSOLUTE)

SET( CTEST_SOURCE_DIRECTORY "${IVP_SOURCE_DIR}" )
SET( CTEST_BINARY_DIRECTORY "${IVP_BUILD_DIR}" )

# Clean out the binary directory before running
#SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

SET( CTEST_PROJECT_SUBPROJECTS "${MOOS_SOURCE_DIR} ${IVP_SOURCE_DIR}" ) 



##############################################################################
#                         END of continuous.cmake
##############################################################################


