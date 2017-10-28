## first.cpp
set (FIRST first)

set (SRCS_FIRST
    first.cpp)

add_executable(${FIRST} ${SRCS_FIRST})

target_link_libraries(
    ${FIRST}
    boost_program_options   ## /usr/local/lib/libboost_program_options.so
)

## multiple_sources.cpp
set (multiple_sources multiple_sources)
set (srcs_multiple_sources
    multiple_sources.cpp)
add_executable(${multiple_sources} ${srcs_multiple_sources})
target_link_libraries(
  ${multiple_sources}
  boost_program_options
  )

target_compile_options( ${multiple_sources} PUBLIC -std=c++11 -fno-rtti)


set_target_properties( ${FIRST} ${multiple_sources}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/program_options"
	)
