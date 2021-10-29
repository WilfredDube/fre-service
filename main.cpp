#include <ev.h>

#include "msgqueue/Mapper.h"
#include "msgqueue/amqp/AMQPEventEmitter.h"
#include "msgqueue/amqp/AMQPEventListener.h"
#include "msgqueue/amqp/AMQPHandler.h"
#include "libfxtract/include/Processor.h"
#include "logging/include/StandardOutputLogger.h"

#include <functional>

using json = nlohmann::json;

enum class ProcessLevel
{
  UNPROCESSED,
  FEATURE_EXTRACTED,
  PROCESS_PLAN_GEN
};

std::string Exchange = "processes";

int main()
{
  // access to the event loop
  auto *loop = EV_DEFAULT;

  // handler for libev
  AMQPHandler handler(loop);

  Logger loggingService = std::make_shared<Fxt::Logging::StandardOutputLogger>("feature-recognition-service");
  loggingService->writeInfoEntry(__FILE__, __LINE__, "Starting feature-recognition-service service....");

  std::string AMQPAddress;
  try
  {
    AMQPAddress = std::getenv("AMQP");
  }
  catch (const std::exception &e)
  {
    loggingService->writeErrorEntry(__FILE__, __LINE__, e.what());
    exit(1);
  }

  // make a connection to AMQP broker
  auto connection = std::make_shared<AMQP::TcpConnection>(&handler, AMQP::Address(AMQPAddress));

  auto eventEmitter = std::make_shared<AMQPEventEmitter>(connection, Exchange, loggingService);
  auto eventListener = std::make_shared<AMQPEventListener>(connection, eventEmitter, Exchange, QueueName, loggingService);

  std::string event("featureRecognitionStarted");
  std::vector<std::string> events;
  events.push_back(event);

  std::function<std::shared_ptr<Event>(EventPtr event, Logger loggingService)> f = ProcessCadFile;

  eventListener->Listen(events, f);

  return 0;
}