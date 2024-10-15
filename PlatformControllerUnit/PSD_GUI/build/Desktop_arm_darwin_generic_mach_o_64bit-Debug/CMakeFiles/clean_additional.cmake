# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/PSD_GUI_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/PSD_GUI_autogen.dir/ParseCache.txt"
  "PSD_GUI_autogen"
  )
endif()
