#include "AMQPEventListener.h"
#include "../Event.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <amqpcpp/linux_tcp/tcpchannel.h>

AMQPEventListener::AMQPEventListener(std::shared_ptr<AMQP::TcpConnection> &connection,
                                     std::shared_ptr<EventEmitter> eventEmitter,
                                     std::string exchange, std::string queue)
    : connection{connection}, eventEmitter{eventEmitter}, exchange{exchange}, queue{queue}
{
    setUp();
}

void AMQPEventListener::setUp()
{
    AMQP::TcpChannel channel(connection.get());
    connection->heartbeat();

    channel.declareExchange(exchange, AMQP::topic, AMQP::durable);

    channel.declareQueue(QueueName, AMQP::durable)
        .onSuccess([](const std::string &name, uint32_t messagecount, uint32_t consumercount)
                   { std::cout << "Connecting to queue: " << name << std::endl; })
        .onError([](const char *message)
                 { std::cout << "Queue declaration failed: " << message << std::endl; });
}

void AMQPEventListener::Listen(std::vector<std::string> &list,
                               std::function<std::shared_ptr<Event>(EventPtr event)> &ProcessCadFile)
{
    AMQP::TcpChannel channel(connection.get());

    auto *loop = EV_DEFAULT;
    for (auto &event : list)
    {
        channel.bindQueue(exchange, queue, event)
            .onError([](const char *message)
                     { std::cout << "Queue binding failed: " << message << std::endl; });
    }

    channel.consume(QueueName, "", AMQP::noack)
        .onReceived([&](
                        const AMQP::Message &message, uint64_t deliveryTag,
                        bool redelivered)
                    {
                        auto mapper = std::make_shared<EventMapper>();
                        auto event = mapper->MapEvent(eventName, message.body(), (int)message.bodySize());

                        // TODO: Feature recognition
                        auto result = ProcessCadFile(event);

                        if (result == nullptr)
                        {
                            std::cerr << "Failed to recognise features" << std::endl;
                            return;
                        }

                        std::cout << "Publishing features..." << std::endl;
                        eventEmitter->Emit(result);
                    })
        .onSuccess([&]() {})
        .onError([](const char *message)
                 { std::cout << "Queue binding failed: " << message << std::endl; })
        .onFinalize([]()
                    { std::cout << "OnFinalize operation failed" << std::endl; });

    std::cout << "Received========================" << std::endl;
    ev_run(loop, 0);
}
