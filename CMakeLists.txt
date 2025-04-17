cmake_minimum_required(VERSION 3.10)
project(hello)

set (CMAKE_CXX_STANDARD 20)
# Compiler flags only
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g \
-O2 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
-Werror=format-security \
-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
-D_GLIBCXX_ASSERTIONS \
-fstack-clash-protection -fstack-protector-strong")

# Linker flags separately
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
-Wl,-z,nodlopen -Wl,-z,noexecstack \
-Wl,-z,relro -Wl,-z,now \
-Wl,--as-needed -Wl,--no-copy-dt-needed-entries")


set (CMAKE_CXX_STANDARD_REQUIRED ON)

# This is geting gunit so you don't have to...
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(hello main.cpp
        httprequest.cpp
        httprequest.hpp)
	
enable_testing()


add_executable(testbinary httprequest.cpp
httprequest.hpp httprequest_test.cpp) 
target_link_libraries(
  testbinary
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(testbinary)