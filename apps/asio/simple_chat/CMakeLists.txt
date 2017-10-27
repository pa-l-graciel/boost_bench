cmake_minimum_required(VERSION 2.6)

## async_tcp_server.cpp
set (t0 chat_server)
set (srcs_t0
    ServerSession.cpp
    ServerMain.cpp)
add_executable(${t0} ${srcs_t0})
target_link_libraries(
  ${t0}
  boost_system
  )
target_compile_options( ${t0} PUBLIC -std=c++11 -fno-rtti)

## async_tcp_client.cpp
set (t1 chat_client)
set (srcs_t1
    ChattingTCPClient.cpp)
add_executable(${t1} ${srcs_t1})
target_link_libraries(
  ${t1}
  boost_system
  boost_thread
  )
target_compile_options( ${t1} PUBLIC -std=c++11 -fno-rtti)





