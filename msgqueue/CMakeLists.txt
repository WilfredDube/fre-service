set(Headers
  ./Emitter.h
  ./Event.h
  ./amqp/AMQPHandler.h
  ./amqp/AMQPEventListener.h
  ./amqp/AMQPEventEmitter.h
  ./contracts/FeatureRecognitionStarted.h
  ./contracts/FeatureRecognitionComplete.h
  ./contracts/ProcessPlanningStarted.h
  ./contracts/ProcessPlanningComplete.h
  
  ./Listener.h
  ./Mapper.h
)

set(Sources
  ./amqp/AMQPEventListener.cpp
  ./amqp/AMQPEventEmitter.cpp
  ./contracts/FeatureRecognitionStarted.cpp
  ./contracts/FeatureRecognitionComplete.cpp
  ./contracts/ProcessPlanningStarted.cpp
  ./contracts/ProcessPlanningComplete.cpp
  ./Mapper.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lm -Wl,-R/usr/local/lib")

add_library(msgqueue STATIC ${Sources} ${Headers})

target_compile_features(msgqueue PUBLIC cxx_std_17)

set_target_properties(msgqueue PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(msgqueue PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(msgqueue frelib ${Boost_LIBRARIES})
