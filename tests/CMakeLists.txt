# Gtest uses global constructors...
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Wno-global-constructors")

macro(add_toolbox_test test_name test_cpp)
  add_executable(${test_name} ${test_cpp})
  set_target_properties(${test_name} PROPERTIES CXX_STANDARD 20)
  set_target_properties(${test_name} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON)
  set_target_properties(${test_name} PROPERTIES CMAKE_POSITION_INDEPENDENT_CODE ON)
  target_compile_definitions(${test_name} PUBLIC TEST_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test_data")
  set_target_properties(
    ${test_name}
    PROPERTIES CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Weverything -Werror=return-type \
                -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-ctad-maybe-unsupported \
                -Wno-exit-time-destructors -Wno-double-promotion -Wno-global-constructors")
  target_link_libraries(${test_name} PUBLIC toolbox gtest)
  if (COMMAND add_unit_test)
    add_unit_test(${test_name} ${test_name})
  else()
    add_test(${test_name} ${test_name})
  endif()
endmacro()

# https://verdantrobotics-jira.atlassian.net/browse/VC-7069
# add_toolbox_test(test_datetime_str_parser test_datetime_str_parser.cpp)
add_toolbox_test(test_lrucache test_lrucache.cpp)
add_toolbox_test(test_file test_file.cpp)
add_toolbox_test(test_rate test_rate.cpp)
add_toolbox_test(test_circularbuffer test_circularbuffer.cpp)
add_toolbox_test(test_json test_json.cpp)
add_toolbox_test(test_box test_box.cpp)
add_toolbox_test(test_split test_split.cpp)
add_toolbox_test(test_strings test_strings.cpp)
add_toolbox_test(test_percentile_buffer test_percentile_buffer.cpp)
