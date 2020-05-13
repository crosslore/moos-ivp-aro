SET(MOOS_GIT_VERSION "not known")
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} describe --abbrev=5 --dirty --always
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "MOOS_GIT_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  ELSE(GIT_FOUND)
#    MESSAGE( STATUS "Git not found: ${MOOS_GIT_VERSION}" )
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)


MESSAGE( STATUS "Git version: ${MOOS_GIT_VERSION}" )

CONFIGURE_FILE(${CMAKE_MODULE_PATH}/GitVersion.h.in 
${CMAKE_CURRENT_SOURCE_DIR}/Core/libMOOS/include/MOOS/libMOOS/GitVersion.h @ONLY)
