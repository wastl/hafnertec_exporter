cmake_minimum_required(VERSION 3.21)
project(hafnertec_exporter)

set(CMAKE_CXX_STANDARD 14)

add_executable(hafnertec_exporter main.cpp hafnertec_client.cpp hafnertec_client.h hafnertec_influx.cpp hafnertec_influx.h)
target_link_libraries(hafnertec_exporter PUBLIC cpprest pthread ssl crypto yaml-cpp glog gumbo gumbo_query absl_strings absl_status absl_throw_delegate)
