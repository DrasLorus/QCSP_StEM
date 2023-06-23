
enable_testing ()

# TEST WITH ZERO GENERATOR

set (LOCAL_RESULT_FILE ${CMAKE_CURRENT_BINARY_DIR}/result_zero_generator.bin)
set (LOCAL_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/output_zero_generator.bin)

configure_file (sources/float_file_comparator/float_file_comparator.cc.in zero_file_comparator.cc)

find_file (ZSTD_BIN zstd REQUIRED)
add_custom_command (
  OUTPUT ${LOCAL_OUTPUT_FILE}
  COMMAND "${ZSTD_BIN}" -f -T0 -d -o "${LOCAL_OUTPUT_FILE}" "${CMAKE_SOURCE_DIR}/data/output_zero_generator.bin.zst"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${CMAKE_SOURCE_DIR}/data/output_zero_generator.bin.zst
  VERBATIM
)

add_custom_command (
  OUTPUT ${LOCAL_RESULT_FILE}
  COMMAND ${CMAKE_COMMAND} -E remove dump_file.bin
  COMMAND emitter_usrp --to-file --no-ui --generator zero -r 1e6 -f 433.92e6 -g 50 -a TX/RX --count 1000
  COMMAND mv dump_file.bin ${LOCAL_RESULT_FILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS emitter_usrp
  VERBATIM
)

add_custom_target(
  zero_test_files DEPENDS ${LOCAL_RESULT_FILE} ${LOCAL_OUTPUT_FILE}
)

add_executable (zero_file_comparator zero_file_comparator.cc)
if (LINUX)
  target_link_libraries (zero_file_comparator PRIVATE m)
endif ()
add_dependencies (zero_file_comparator zero_test_files)

add_test (
  NAME "The transmitter output for the CZeroGenerator is as expected."
  COMMAND zero_file_comparator
)
