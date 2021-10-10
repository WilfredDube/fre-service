/**
 *  LibEV.cpp
 * 
 *  Test program to check AMQP functionality based on LibEV
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2015 - 2018 Copernica BV
 */

/**
 *  Dependencies
 */
#include <ev.h>

#include "msgqueue/Mapper.h"
#include "msgqueue/amqp/AMQPEventEmitter.h"
#include "msgqueue/amqp/AMQPEventListener.h"
#include "msgqueue/amqp/AMQPHandler.h"
#include "libfxtract/include/Processor.h"
#include "logging/include/StandardOutputLogger.h"

#include <functional>
// namespace fs = std::filesystem;

using json = nlohmann::json;

enum class ProcessLevel
{
  UNPROCESSED,
  FEATURE_EXTRACTED,
  PROCESS_PLAN_GEN
};

std::string AMQPMessageBroker = "amqp://guest:guest@rabbitmq";
std::string Exchange = "processes";
std::string AMQPMessageExternalBroker = "amqp://guest:guest@localhost:5672";

/**
 *  Main program
 *  @return int
 */
int main()
{
  // access to the event loop
  auto *loop = EV_DEFAULT;

  // handler for libev
  AMQPHandler handler(loop);

  Logger loggingService = std::make_shared<Fxt::Logging::StandardOutputLogger>("feature-recognition-service");
  loggingService->writeInfoEntry(__FILE__, __LINE__, "Starting feature-recognition-service service....");


  auto eventEmitter = std::make_shared<AMQPEventEmitter>(connection, Exchange, loggingService);
  auto eventListener = std::make_shared<AMQPEventListener>(connection, eventEmitter, Exchange, QueueName, loggingService);

  std::string event("featureRecognitionStarted");
  std::vector<std::string> events;
  events.push_back(event);

  std::function<std::shared_ptr<Event>(EventPtr event, Logger loggingService)> f = ProcessCadFile;

  eventListener->Listen(events, f);

  // done
  return 0;
}