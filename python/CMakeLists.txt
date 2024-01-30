pybind11_add_module (
  qcsp
  SHARED
  sources/main_python_wrapper.cpp
  ${CMAKE_SOURCE_DIR}/sources/utilities/conversions.cpp
  ${CMAKE_SOURCE_DIR}/sources/CQCSPModulator/CCompleteModulator/CCompleteModulator.cpp
  ${CMAKE_SOURCE_DIR}/sources/CQCSPModulator/CQCSPModulator.cpp
)
target_include_directories (
  qcsp SYSTEM
  PUBLIC ${CMAKE_SOURCE_DIR}/sources
         sources
         ${Python3_INCLUDE_DIRS}
         ${pybind11_INCLUDE_DIR}
)
target_link_libraries (qcsp PUBLIC pybind11::headers)
set_target_properties (
  qcsp
  PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN ON
)
