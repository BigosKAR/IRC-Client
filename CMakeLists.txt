cmake_minimum_required(VERSION 3.5)

set(SOURCES
    src/main.c
    src/accounts.c
    src/formatting.c
)

add_executable(irc_client ${SOURCES})
target_link_libraries(irc_client ws2_32)