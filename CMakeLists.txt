cmake_minimum_required( VERSION 3.12.0 )

# Ensure all our binaries conform with C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchDependency)

fetch_dependency( vlog "https://git.martian.ag/open-source/vlog.cbuf" "master" )

find_package( hjson REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

add_library( toolbox SHARED src/file_utils.cpp src/hjson_helper src/unix_helper src/stb_image_write )
set_target_properties(toolbox PROPERTIES CXX_STANDARD 17)
target_include_directories( toolbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/toolbox )
target_include_directories( toolbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_link_libraries( toolbox PUBLIC hjson vlog stdc++fs)

install(TARGETS toolbox DESTINATION lib)
