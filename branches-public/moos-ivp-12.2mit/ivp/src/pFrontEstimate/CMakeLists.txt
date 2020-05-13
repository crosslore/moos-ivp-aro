#--------------------------------------------------------
# The CMakeLists.txt for:                  pFrontEstimate
# Author(s):                               Henrik Schmidt
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pFrontEstimate ${SRC})
   
TARGET_LINK_LIBRARIES(pFrontEstimate
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   henrik_util
)
