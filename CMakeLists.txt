cmake_minimum_required(VERSION 3.5)

add_executable(irc_client main.c)
target_link_libraries(irc_client ws2_32)