#include "AMQPEventListener.h"
#include "../Event.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <amqpcpp/linux_tcp/tcpchannel.h>

AMQPEventListener::AMQPEventListener(std::shared_ptr<AMQP::TcpConnection> &connection,
                                     std::shared_ptr<EventEmitter> eventEmitter,
                                     std::string exchange, std::string queue, Logger loggingService)
    : connection{connection}, eventEmitter{eventEmitter}, exchange{exchange}, queue{queue}, loggingService{loggingService}
{
    setUp();
}

void AMQPEventListener::setUp()
{
    AMQP::TcpChannel channel(connection.get());
    connection->heartbeat();

    channel.declareExchange(exchange, AMQP::topic, AMQP::durable);

    channel.declareQueue(QueueName, AMQP::durable)
        .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount)
                   { loggingService->writeInfoEntry(__FILE__, __LINE__, "Connecting to queue: " + name); })
        .onError([&](const char *message)
                 { loggingService->writeErrorEntry(__FILE__, __LINE__, "Queue declaration failed: " + std::string(message)); });
}

void AMQPEventListener::Listen(std::vector<std::string> &list,
                               std::function<std::shared_ptr<Event>(EventPtr event, Logger loggingService)> &ProcessCadFile)
{
    AMQP::TcpChannel channel(connection.get());

    auto *loop = EV_DEFAULT;
    for (auto &event : list)
    {
        channel.bindQueue(exchange, queue, event)
            .onError([&](const char *message)
                     { loggingService->writeErrorEntry(__FILE__, __LINE__, "Queue binding failed: " + std::string(message)); });
    }

    channel.consume(QueueName, "", AMQP::noack)
        .onReceived([&](
                        const AMQP::Message &message, uint64_t deliveryTag,
                        bool redelivered)
                    {
                        loggingService->writeInfoEntry(__FILE__, __LINE__, "Starting feature recognition....");
                        auto mapper = std::make_shared<EventMapper>();
                        auto event = mapper->MapEvent(eventName, message.body(), (int)message.bodySize());

                        loggingService->writeInfoEntry(__FILE__, __LINE__, "Mapped...");

                        auto result = ProcessCadFile(event, loggingService);

                        loggingService->writeInfoEntry(__FILE__, __LINE__, "Extracted...");
                        if (result == nullptr)
                        {
                            loggingService->writeErrorEntry(__FILE__, __LINE__, "Failed to extract feature");
                            return;
                        }

                        loggingService->writeInfoEntry(__FILE__, __LINE__, "Publishing feature....");
                        eventEmitter->Emit(result);
                    })
        .onSuccess([&]() {})
        .onError([](const char *message)
                 { std::cout << "Queue binding failed: " << message << std::endl; })
        .onFinalize([&]()
                    { loggingService->writeErrorEntry(__FILE__, __LINE__, "OnFinalize operation failed"); });

    ev_run(loop, 0);
}
