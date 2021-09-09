cmake_minimum_required(VERSION 3.0)

project(fre-service_1.0)

link_libraries(stdc++fs)

add_subdirectory(logging)
add_subdirectory(libfxtract)
add_subdirectory(msgqueue)
#add_subdirectory(tests)

set(main_source_files
 main.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF((CMAKE_CXX_STANDARD EQUAL 17) OR (CMAKE_CXX_STANDARD GREATER 17))
SET(WT_CPP17_ANY_DEFAULT_IMPLEMENTATION "std")
ENDIF()

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -Wall -lm -Wl,-R/usr/local/lib")

find_package(amqpcpp REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_executable (fre-service ${main_source_files})
target_include_directories(fre-service PRIVATE ${amqpcpp_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(fre-service logger frelib msgqueue -g ${Boost_LIBRARIES} Threads::Threads -lboost_serialization ev boost_system dl Threads::Threads ${amqpcpp_LIBRARIES} amqpcpp nlohmann_json::nlohmann_json)

install(
  TARGETS fre-service 
  DESTINATION bin
)

#target_include_directories(fxtract PRIVATE ${Boost_INCLUDE_DIRS})
#target_link_libraries(fxtract logger frelib seqgen -g ${Boost_LIBRARIES} Threads::Threads -lboost_serialization)
