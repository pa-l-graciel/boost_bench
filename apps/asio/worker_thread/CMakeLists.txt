cmake_minimum_required(VERSION 2.6)

## using background job manager for async i/o
set (t0 workerthread)
set (srcs_t0
    workerthread.cpp)
add_executable(${t0} ${srcs_t0})
target_link_libraries(
  ${t0}
  boost_system
  boost_thread
#  boost_chrono
  )
#target_compile_options( ${t0} PUBLIC -std=c++11 -fno-rtti)

set_target_properties( ${t0}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/asio"
	)
