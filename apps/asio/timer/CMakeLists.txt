cmake_minimum_required(VERSION 2.6)

## steady timer
set (t0 timer1)
set (srcs_t0
    timer1.cpp)
add_executable(${t0} ${srcs_t0})
target_link_libraries(
  ${t0}
  boost_system
  boost_chrono
  )
#target_compile_options( ${t0} PUBLIC -std=c++11 -fno-rtti)

## timer2
set (t1 timer2)
set (srcs_t1
    timer2.cpp)
add_executable(${t1} ${srcs_t1})
target_link_libraries(
  ${t1}
  boost_system
  boost_chrono
  )
#target_compile_options( ${t0} PUBLIC -std=c++11 -fno-rtti)


