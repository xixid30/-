# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "RelWithDebInfo")
  file(REMOVE_RECURSE
  "CMakeFiles\\calculator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\calculator_autogen.dir\\ParseCache.txt"
  "calculator_autogen"
  )
endif()
