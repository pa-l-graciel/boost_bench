cmake_minimum_required(VERSION 2.6)

## debug_settings.cpp
if (FALSE)
set (debug_settings debug_settings)
set (srcs_debug_settings
    debug_settings.cpp)
add_executable(${debug_settings} ${srcs_debug_settings})
target_link_libraries(
  ${debug_settings}
  boost_program_options
  )
endif()



## access_data.cpp
if (FALSE)
set (access_data access_data)
set (srcs_access_data
    access_data.cpp)
#set(CMAKE_DEFS "-std=c++0x")
set(defs_access_data "-std=c++11")
add_definitions(${defs_access_data})
add_executable(${access_data} ${srcs_access_data})
target_link_libraries(
  ${access_data}
  boost_program_options
  )
endif()

## speed_test.cpp
if (FALSE)
set (speed_test speed_test)
set (srcs_speed_test
    speed_test.cpp)
set(defs_speed_test "-std=c++11 -O3")
add_definitions(${defs_speed_test})
add_executable(${speed_test} ${srcs_speed_test})
target_link_libraries(
  ${speed_test}
  boost_program_options
  )
endif()


## json.cpp
set (json json)
set (srcs_json
    json.cpp)
add_executable(${json} ${srcs_json})
target_link_libraries(
  ${json}
  boost_program_options
  )
#target_compile_options( ${json} PUBLIC -std=c++11 -fno-rtti)
#target_compile_options( ${json} PUBLIC -std=c++11 -frtti -g -O2)
target_compile_options( ${json} PUBLIC -frtti -g -O2)

set_target_properties( ${json}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/property_tree"
	)

add_custom_command(
	TARGET ${json} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	#	${CMAKE_CURRENT_BINARY_DIR}/input.json   # src path
	./input.json   # src path
	${CMAKE_SOURCE_DIR}/bin/property_tree    # dst path
	)


set (copy_list
	./*.json   # src path
	./*.xml
	./*.info
)

foreach(file_i ${copy_list})
	add_custom_command(
		TARGET ${json} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${file_i}
		${CMAKE_SOURCE_DIR}/bin/property_tree    # dst path
	)
endforeach(file_i)


