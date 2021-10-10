#include "../Emitter.h"
#include "../Event.h"
#include "../../logging/include/LoggingFacility.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>

struct AMQPEventEmitter : public EventEmitter
{
    std::shared_ptr<AMQP::TcpConnection> connection;
    std::string exchange;
    AMQP::Table arguments;
    Logger loggingService;

public:
    AMQPEventEmitter(std::shared_ptr<AMQP::TcpConnection> connection,
                     std::string exchange, Logger loggingService);

    void setUp();

    bool Emit(std::shared_ptr<Event> event);
};