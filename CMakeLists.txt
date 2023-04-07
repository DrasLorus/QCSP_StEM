cmake_minimum_required (VERSION 3.16)

# Configure Compiler ##############################################################################
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (QCSP_Standalone_Emitter LANGUAGES CXX)

# Options #########################################################################################
option (UNSAFE_OPTIMIZATION "enable unsafe flags" OFF)
option (PREFER_STATIC_LIBRARY "prefer static libraries" ON)

option (PROFILING "enable GProf profiling" OFF)

option (ENABLE_NEMATODE "use of the NemaTode C++11 library to read GNSS frame" ON)
option (ENABLE_TESTING "enable unit-testing with Catch2" OFF)

# Find dependencies ###############################################################################
find_package (UHD REQUIRED)
find_package (Boost REQUIRED COMPONENTS program_options)
find_package (Threads REQUIRED)

find_library (matio NAMES matio REQUIRED)

find_library (
  fftw
  NAMES fftw fftw3
  HINTS "/usr/local/lib" "/usr/local" REQUIRED
)

find_library (
  fftwf
  NAMES fftwf fftw3f
  HINTS "/usr/local/lib" "/usr/local" REQUIRED
)

if (ENABLE_NEMATODE)
  add_subdirectory (gps-nematode)
  add_compile_definitions (USE_NEMATODE=1)
endif ()

if (ENABLE_TESTING)
  find_package (Catch2 REQUIRED)
endif ()

add_executable (
  emitter_usrp
  sources/main_usrp_emitter.cpp
  sources/threads/user_interface.cpp
  sources/threads/timer.cpp
  sources/utilities/hmi_functions.cpp
  sources/utilities/usrp_functions.cpp
  sources/CFftEngine/CFftComplexEngine.cpp
)
target_include_directories (emitter_usrp PUBLIC sources)
target_include_directories (emitter_usrp SYSTEM PUBLIC ${UHD_INCLUDE_DIRS})
target_link_libraries (
  emitter_usrp
  PRIVATE ${UHD_LIBRARIES}
          Boost::program_options
          matio
          ${fftw}
          ${fftwf}
          Threads::Threads
)

if (ENABLE_NEMATODE)
  target_sources (emitter_usrp PRIVATE sources/GPSReader/GPSReader.cpp)
  target_include_directories (emitter_usrp SYSTEM PRIVATE gps-nematode/include)
  target_link_libraries (emitter_usrp PRIVATE NemaTode)
endif ()
