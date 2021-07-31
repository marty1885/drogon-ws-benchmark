cmake_minimum_required(VERSION 3.5)
project(drogon_ws_benchmark)
find_package(Drogon CONFIG REQUIRED)

add_executable(ws_bench_server server.cpp)
target_link_libraries(ws_bench_server PRIVATE Drogon::Drogon)

add_executable(ws_bench_client client.cpp)
target_link_libraries(ws_bench_client PRIVATE Drogon::Drogon)
