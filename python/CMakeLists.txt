pybind11_add_module (qcsp_stem SHARED sources/main_python_wrapper.cpp)
target_include_directories (qcsp_stem SYSTEM PUBLIC ${Python3_INCLUDE_DIRS} ${pybind11_INCLUDE_DIR})
target_link_libraries (qcsp_stem PUBLIC pybind11::headers qcsp_modulator_obj)
set_target_properties (
  qcsp_stem
  PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN ON
)
