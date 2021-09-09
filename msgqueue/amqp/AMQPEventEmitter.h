#include "../Emitter.h"
#include "../Event.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>

struct AMQPEventEmitter : public EventEmitter
{
    std::shared_ptr<AMQP::TcpConnection> connection;
    std::string exchange;
    AMQP::Table arguments;

public:
    AMQPEventEmitter(std::shared_ptr<AMQP::TcpConnection> connection,
                     std::string exchange);

    void setUp();

    bool Emit(std::shared_ptr<Event> event);
};