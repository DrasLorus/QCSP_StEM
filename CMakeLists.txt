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

set (QCSP_GF_ORDER "6" CACHE STRING "Order of the Galois Field and bits per symbol (p = log2(q))")
set_property (CACHE QCSP_GF_ORDER PROPERTY STRINGS "6")

set (QCSP_N_SYMBOLS "60" CACHE STRING "Number of symbols in a frame (N = K / Rc)")
set_property (CACHE QCSP_N_SYMBOLS PROPERTY STRINGS "60")

set (QCSP_K_SYMBOLS "20" CACHE STRING "Number of symbols in a frame (K = N * Rc)")
set_property (CACHE QCSP_K_SYMBOLS PROPERTY STRINGS "20")

set (QCSP_NBLDPC_MATID "N${QCSP_N_SYMBOLS}_K${QCSP_K_SYMBOLS}_p${QCSP_GF_ORDER}" CACHE INTERNAL "Varable used to select the right constant headers." FORCE)

configure_file (sources/utilities/definitions.hpp.in ${CMAKE_SOURCE_DIR}/sources/utilities/definitions.hpp @ONLY)
configure_file (sources/CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp.in ${CMAKE_SOURCE_DIR}/sources/CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp @ONLY)

add_executable (
  emitter_usrp
  sources/main_usrp_emitter.cpp
  sources/threads/user_interface.cpp
  sources/threads/timer.cpp
  sources/utilities/hmi_functions.cpp
  sources/utilities/usrp_functions.cpp
  sources/CFftEngine/CFftComplexEngine.cpp
  sources/CSymbolGenerator/CSymbolGenerator.cpp
  sources/CQCSPModulator/CQCSPModulator.cpp
  sources/CQCSPModulator/CCompleteModulator/CCompleteModulator.cpp
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
  target_sources (emitter_usrp PRIVATE sources/CSymbolGenerator/GPSGenerator/GPSGenerator.cpp)
  target_include_directories (emitter_usrp SYSTEM PRIVATE gps-nematode/include)
  target_link_libraries (emitter_usrp PRIVATE NemaTode)
endif ()
