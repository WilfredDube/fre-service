set(Headers
  include/cloud.h
  include/URL.h
)

set(Sources
  src/cloud.cpp
  src/URL.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lm -Wl,-R/usr/local/lib")

set(CURL_LIBRARY "-lcurl") 
find_package(CURL REQUIRED) 

add_library(cloud STATIC ${Sources} ${Headers})

target_compile_features(cloud PUBLIC cxx_std_17)

set_target_properties(cloud PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(cloud PUBLIC ${CURL_INCLUDE_DIR})
target_link_libraries(cloud ${CURL_LIBRARIES} azure-storage-lite -lcrypto -luuid)
