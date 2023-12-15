enable_testing ()

# TEST WITH ZERO GENERATOR

set (LOCAL_RESULT_FILE ${CMAKE_CURRENT_BINARY_DIR}/result_zero_generator.bin)
set (LOCAL_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/output_zero_generator.bin)

configure_file (sources/float_file_comparator/float_file_comparator.cc.in zero_file_comparator.cc)

find_program (ZSTD_BIN zstd REQUIRED)

set (
  LOCAL_CMDLINE_ZSTD
  ${ZSTD_BIN};-f;-T0;-d;-o;${LOCAL_OUTPUT_FILE};${CMAKE_SOURCE_DIR}/data/output_zero_generator.bin.zst
)

add_custom_command (
  OUTPUT ${LOCAL_OUTPUT_FILE}
  COMMAND ${LOCAL_CMDLINE_ZSTD}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${CMAKE_SOURCE_DIR}/data/output_zero_generator.bin.zst
  COMMAND_EXPAND_LISTS
)

set (
  LOCAL_CMDLINE_ZERO
  $<TARGET_FILE:qcsp_standalone_emitter>;--to-file;--output-file;${LOCAL_RESULT_FILE};--no-ui;--generator;zero;-r;1e6;-f;433.92e6;-g;50;-a;TX/RX;--count;1000
)

add_custom_command (
  OUTPUT ${LOCAL_RESULT_FILE}
  COMMAND ${CMAKE_COMMAND} -E remove ${LOCAL_RESULT_FILE}
  COMMAND ${LOCAL_CMDLINE_ZERO}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS qcsp_standalone_emitter
  COMMAND_EXPAND_LISTS
)

add_custom_target (zero_test_files DEPENDS ${LOCAL_RESULT_FILE} ${LOCAL_OUTPUT_FILE})

add_executable (zero_file_comparator zero_file_comparator.cc)
if (LINUX)
  target_link_libraries (zero_file_comparator PRIVATE m)
endif ()
add_dependencies (zero_file_comparator zero_test_files)

add_test (NAME "The transmitter output for the CZeroGenerator is as expected."
          COMMAND zero_file_comparator
)

# TEST WITH STDIN GENERATOR

set (LOCAL_INPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/random_data_00.bin)
set (LOCAL_RESULT_FILE ${CMAKE_CURRENT_BINARY_DIR}/result_stdin_random_data_00.bin)
set (LOCAL_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/output_random_data_00.bin)

configure_file (
  sources/float_file_comparator/float_file_comparator.cc.in random_data_stdin_comparator.cc
)

add_custom_command (
  OUTPUT ${LOCAL_OUTPUT_FILE} ${LOCAL_INPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E tar -xvf "${CMAKE_SOURCE_DIR}/data/random_data_test.tar.zst"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${CMAKE_SOURCE_DIR}/data/random_data_test.tar.zst
)

add_custom_target (random_data_archive_files DEPENDS ${LOCAL_INPUT_FILE} ${LOCAL_OUTPUT_FILE})

set (
  LOCAL_CMDLINE
  $<TARGET_FILE:qcsp_standalone_emitter>;--to-file;--output-file;${LOCAL_RESULT_FILE};--no-ui;--generator;stdin;-r;1e6;-f;433.92e6;-g;50;-a;TX/RX;--count;10000
)

add_custom_command (
  OUTPUT ${LOCAL_RESULT_FILE}
  COMMAND ${CMAKE_COMMAND} -E remove ${LOCAL_RESULT_FILE}
  COMMAND ${CMAKE_COMMAND} -E cat ${LOCAL_INPUT_FILE} | ${LOCAL_CMDLINE}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS qcsp_standalone_emitter random_data_archive_files
  COMMAND_EXPAND_LISTS
)

add_custom_target (
  random_data_stdin_test_files DEPENDS ${LOCAL_RESULT_FILE} random_data_archive_files
)

add_executable (random_data_stdin_comparator random_data_stdin_comparator.cc)
if (LINUX)
  target_link_libraries (random_data_stdin_comparator PRIVATE m)
endif ()
add_dependencies (random_data_stdin_comparator random_data_stdin_test_files)

add_test (
  NAME "The transmitter output for random data (10^4 frames) using CCinReaderGenerator is as expected."
  COMMAND random_data_stdin_comparator
)

# TEST WITH FILE GENERATOR

set (LOCAL_RESULT_FILE ${CMAKE_CURRENT_BINARY_DIR}/result_file_random_data_00.bin)

configure_file (
  sources/float_file_comparator/float_file_comparator.cc.in random_data_file_comparator.cc
)

set (
  LOCAL_CMDLINE
  $<TARGET_FILE:qcsp_standalone_emitter>;--to-file;--output-file;${LOCAL_RESULT_FILE};--no-ui;--generator;file;--input-file;${LOCAL_INPUT_FILE};-r;1e6;-f;433.92e6;-g;50;-a;TX/RX;--count;10000
)
add_custom_command (
  OUTPUT ${LOCAL_RESULT_FILE}
  COMMAND ${CMAKE_COMMAND} -E remove ${LOCAL_RESULT_FILE}
  COMMAND ${LOCAL_CMDLINE}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS qcsp_standalone_emitter random_data_archive_files
  COMMAND_EXPAND_LISTS
)

add_custom_target (
  random_data_file_test_files DEPENDS ${LOCAL_RESULT_FILE} random_data_archive_files
)

add_executable (random_data_file_comparator random_data_file_comparator.cc)
if (LINUX)
  target_link_libraries (random_data_file_comparator PRIVATE m)
endif ()
add_dependencies (random_data_file_comparator random_data_file_test_files)

add_test (
  NAME "The transmitter output for random data (10^4 frames) using CFileReaderGenerator is as expected."
  COMMAND random_data_file_comparator
)
