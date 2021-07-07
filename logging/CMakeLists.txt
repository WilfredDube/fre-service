set(Headers
  include/LoggingFacility.h
  include/StandardOutputLogger.h
  include/FileSystemLogger.h
)

set(Sources
  src/StandardOutputLogger.cpp
  src/FileSystemLogger.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lm -Wl,-R/usr/local/lib")

add_library(logger STATIC ${Sources} ${Headers})

target_compile_features(logger PUBLIC cxx_std_17)

set_target_properties(logger PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(logger PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(logger ${Boost_LIBRARIES})
