cmake_minimum_required(VERSION 2.6)

#if (FALSE)
## sync_tcp_server.cpp
set (t0 sync_tcp_server)
set (srcs_t0
    sync_tcp_server.cpp)
add_executable(${t0} ${srcs_t0})
target_link_libraries(
  ${t0}
  boost_system
  )
target_compile_options( ${t0} PUBLIC -std=c++11 -fno-rtti)

## sync_tcp_client.cpp
set (t1 sync_tcp_client)
set (srcs_t1
    sync_tcp_client.cpp)
add_executable(${t1} ${srcs_t1})
target_link_libraries(
  ${t1}
  boost_system
  )
target_compile_options( ${t1} PUBLIC -std=c++11 -fno-rtti)
#endif()

## async_tcp_server.cpp
set (t2 async_tcp_server)
set (srcs_t2
    async_tcp_server.cpp)
add_executable(${t2} ${srcs_t2})
target_link_libraries(
  ${t2}
  boost_system
  )
target_compile_options( ${t2} PUBLIC -std=c++11 -fno-rtti)

## async_tcp_client.cpp
set (t3 async_tcp_client)
set (srcs_t3
    async_tcp_client.cpp)
add_executable(${t3} ${srcs_t3})
target_link_libraries(
  ${t3}
  boost_system
  )
target_compile_options( ${t3} PUBLIC -std=c++11 -fno-rtti)





