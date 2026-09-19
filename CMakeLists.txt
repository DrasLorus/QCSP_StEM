cmake_minimum_required (VERSION 3.16)

# Configure Compiler ##############################################################################
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set (CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

project (QCSP_StEm LANGUAGES CXX)

# Options #########################################################################################
option (UNSAFE_OPTIMIZATION "enable unsafe flags" OFF)
option (PREFER_STATIC_LIBRARY "prefer static libraries" ON)

option (PROFILING "enable GProf profiling" OFF)

option (ENABLE_TESTING "enable unit-testing with Catch2" OFF)

option (ENABLE_MKL "enable use of intel MKL" OFF)

option (ENABLE_PYTHON_API "enable build of Python APIusing pybind11" ON)

option (ENABLE_DOCUMENTATION "enable build of Doxygen documentation" ON)

# Find dependencies ###############################################################################
cmake_policy (SET CMP0167 NEW)
find_package (UHD REQUIRED)
find_package (Boost REQUIRED COMPONENTS program_options)
find_package (Threads REQUIRED)

find_library (matio NAMES matio REQUIRED)

if (ENABLE_MKL)
  set (MKL_INTERFACE_FULL intel_lp64)
  set (MKL_THREADING intel_thread)
  find_package (MKL CONFIG REQUIRED)
else ()
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
endif ()

add_subdirectory (extern/gps-nematode EXCLUDE_FROM_ALL)
add_compile_definitions (USE_NEMATODE=1)

if (ENABLE_TESTING)
  find_package (Catch2 REQUIRED)
endif ()

set (
  QCSP_GF_ORDER
  "6"
  CACHE STRING "Order of the Galois Field and bits per symbol (p = log2(q))"
)
set_property (CACHE QCSP_GF_ORDER PROPERTY STRINGS "6")

set (
  QCSP_N_SYMBOLS
  "60"
  CACHE STRING "Number of symbols in a frame (N = K / Rc)"
)
set_property (CACHE QCSP_N_SYMBOLS PROPERTY STRINGS "60")

set (
  QCSP_K_SYMBOLS
  "20"
  CACHE STRING "Number of symbols in a frame (K = N * Rc)"
)
set_property (CACHE QCSP_K_SYMBOLS PROPERTY STRINGS "20")

set (
  QCSP_NBLDPC_MATID
  "N${QCSP_N_SYMBOLS}_K${QCSP_K_SYMBOLS}_p${QCSP_GF_ORDER}"
  CACHE INTERNAL "Variable used to select the right constant headers." FORCE
)

configure_file (
  sources/utilities/definitions.hpp.in ${CMAKE_SOURCE_DIR}/sources/utilities/definitions.hpp @ONLY
)
configure_file (
  sources/CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp.in
  ${CMAKE_SOURCE_DIR}/sources/CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp @ONLY
)

add_library (
  qcsp_utils_obj OBJECT sources/utilities/conversions.cpp sources/utilities/hmi_functions.cpp
                        sources/utilities/usrp_functions.cpp
)
target_include_directories (qcsp_utils_obj PUBLIC sources)
target_include_directories (qcsp_utils_obj SYSTEM PUBLIC ${UHD_INCLUDE_DIRS})
target_link_libraries (qcsp_utils_obj PUBLIC ${UHD_LIBRARIES})

add_library (qcsp_fft_obj OBJECT sources/CFftEngine/CFftwDftEngine.cpp sources/CConvEngine/CHalfCorrEngine.cpp)
target_include_directories (qcsp_fft_obj PUBLIC sources)
if (ENABLE_MKL)
  target_compile_options (qcsp_fft_obj PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_COMPILE_OPTIONS>)
  target_include_directories (
    qcsp_fft_obj PUBLIC $<TARGET_PROPERTY:MKL::MKL,INTERFACE_INCLUDE_DIRECTORIES>
  )
  target_link_libraries (qcsp_fft_obj PUBLIC $<LINK_ONLY:MKL::MKL>)
else ()
  target_link_libraries (qcsp_fft_obj PUBLIC ${fftw} ${fftwf})
endif ()

add_library (
	qcsp_modulator_obj STATIC
  sources/CQCSPModulator/CQCSPModulator.cpp
  sources/CQCSPModulator/CCompleteModulator/CCompleteModulator.cpp
  sources/CQCSPModulator/CFakeEncoderModulator/CFakeEncoderModulator.cpp
)
target_include_directories (qcsp_modulator_obj PUBLIC sources)

add_library (
  qcsp_generator_obj OBJECT
  sources/CSymbolGenerator/CSymbolGenerator.cpp
  sources/CSymbolGenerator/CTimerGenerator/CTimerGenerator.cpp
  sources/CSymbolGenerator/CZeroGenerator/CZeroGenerator.cpp
  sources/CSymbolGenerator/CGPSGenerator/CGPSGenerator.cpp
  sources/CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.cpp
  sources/CSymbolGenerator/CCinReaderGenerator/CCinReaderGenerator.cpp
  sources/CSymbolGenerator/CFileReaderGenerator/CFileReaderGenerator.cpp
)
target_include_directories (qcsp_generator_obj PUBLIC sources)
target_include_directories (qcsp_generator_obj SYSTEM PUBLIC extern/gps-nematode/include)
target_link_libraries (qcsp_generator_obj PUBLIC NemaTode qcsp_utils_obj)

add_executable (
  qcsp_standalone_emitter sources/main.cpp sources/threads/user_interface.cpp
                          sources/threads/timer.cpp
)
target_include_directories (qcsp_standalone_emitter PUBLIC sources)
target_link_libraries (
  qcsp_standalone_emitter
  PRIVATE Boost::program_options
          matio
          Threads::Threads
          qcsp_fft_obj
          qcsp_utils_obj
          qcsp_generator_obj
          qcsp_modulator_obj
)

if (ENABLE_TESTING)
  enable_testing ()
  cmake_policy (SET CMP0110 NEW)
  add_subdirectory (testsuite)
endif ()

if (ENABLE_PYTHON_API)
  find_package (Python3 COMPONENTS Interpreter Development)
  add_subdirectory (extern/pybind11)

  add_subdirectory (python)
endif ()

if (ENABLE_DOCUMENTATION)
  add_subdirectory (documentation)
endif ()

include (GNUInstallDirs)
install (TARGETS qcsp_standalone_emitter)
install (FILES data/parameters_20210903.mat DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
if (ENABLE_DOCUMENTATION)
	install (DIRECTORY ${CMAKE_SOURCE_DIR}/documentation/doxygen/html TYPE DOC)
endif ()
