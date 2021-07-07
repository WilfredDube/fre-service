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
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <amqpcpp/linux_tcp/tcpchannel.h>
#include <nlohmann/json.hpp>

#include "./libfxtract/include/sheet-metal-component/BoostSerializer.h"
#include "./libfxtract/include/cad-file-reader/CadFileReader.h"
#include "./libfxtract/include/cad-file-reader/CadFileReaderFactory.h"
#include "./logging/include/LoggingFacility.h"
#include "./logging/include/StandardOutputLogger.h"

#include <filesystem>

namespace fs = std::filesystem;

using json = nlohmann::json;

enum class ProcessLevel
{
  UNPROCESSED,
  FEATURE_EXTRACTED,
  PROCESS_PLAN_GEN
};

json processMessage(std::string message, int size);

// Use a config file
std::string rabbit_host = "localhost"; //os.Getenv("RABBIT_HOST")
std::string rabbit_port = "5672";      //os.Getenv("RABBIT_PORT")
std::string rabbit_user = "guest";     //os.Getenv("RABBIT_USERNAME")
std::string rabbit_password = "guest"; //os.Getenv("RABBIT_PASSWORD")

/**
 *  Custom handler
 */
class MyHandler : public AMQP::LibEvHandler
{
private:
  /**
     *  Method that is called when a connection error occurs
     *  @param  connection
     *  @param  message
     */
  virtual void onError(AMQP::TcpConnection *connection, const char *message) override
  {
    std::cout << "error: " << message << std::endl;
  }

  /**
     *  Method that is called when the TCP connection ends up in a connected state
     *  @param  connection  The TCP connection
     */
  virtual void onConnected(AMQP::TcpConnection *connection) override
  {
    std::cout << "connected" << std::endl;
  }

  /**
     *  Method that is called when the TCP connection ends up in a ready
     *  @param  connection  The TCP connection
     */
  virtual void onReady(AMQP::TcpConnection *connection) override
  {
    std::cout << "ready" << std::endl;
  }

  /**
     *  Method that is called when the TCP connection is closed
     *  @param  connection  The TCP connection
     */
  virtual void onClosed(AMQP::TcpConnection *connection) override
  {
    std::cout << "closed" << std::endl;
  }

  /**
     *  Method that is called when the TCP connection is detached
     *  @param  connection  The TCP connection
     */
  virtual void onDetached(AMQP::TcpConnection *connection) override
  {
    std::cout << "detached" << std::endl;
  }

public:
  /**
     *  Constructor
     *  @param  ev_loop
     */
  MyHandler(struct ev_loop *loop) : AMQP::LibEvHandler(loop) {}

  /**
     *  Destructor
     */
  virtual ~MyHandler() = default;
};

/**
 *  Main program
 *  @return int
 */
int main()
{
  // access to the event loop
  auto *loop = EV_DEFAULT;

  // handler for libev
  MyHandler handler(loop);

  // init the SSL library
  // #if OPENSSL_VERSION_NUMBER < 0x10100000L
  //   SSL_library_init();
  // #else
  //   OPENSSL_init_ssl(0, NULL);
  // #endif

  // make a connection
  AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://" + rabbit_user + ":" + rabbit_password + "@" + rabbit_host + ":" + rabbit_port + "/"));
  // AMQP::Address address("amqp://guest:guest@localhost/");
  //    AMQP::Address address("amqps://guest:guest@localhost/");
  // AMQP::TcpConnection connection(&handler, address);

  // we need a channel too
  AMQP::TcpChannel channel(&connection);
  connection.heartbeat();

  // Define callbacks and start
  auto messageCb = [&](
                       const AMQP::Message &message, uint64_t deliveryTag,
                       bool redelivered) {
    std::cout << "message received" << std::endl;
    std::cout << "Size: " << message.bodySize() << " bytes" << std::endl;

    // acknowledge the message
    auto res = processMessage(message.body(), message.bodySize());

    if (res.empty() != true)
      channel.publish("", "FRECRESPONSE", res.dump());

    std::cout << "===================================: " << message.replyTo() << std::endl;
    // channel.ack(deliveryTag);
    // publishResponse(res);
  };

  // callback function that is called when the consume operation starts
  auto startCb = [](const std::string &consumertag) {
    std::cout << "consume operation started: " << consumertag << std::endl;
  };

  // callback function that is called when the consume operation failed
  auto errorCb = [](const char *message) {
    std::cout << "consume operation failed: " << message << std::endl;
  };

  channel.consume("FREC", AMQP::noack)
      .onReceived(messageCb)
      .onSuccess(startCb)
      .onError(errorCb)
      .onFinalize([]() {
        std::cout << "OnFinalize operation failed" << std::endl;
      });

  AMQP::Table arguments;
  // arguments["x-message-ttl"] = 120 * 1000;

  // declare the queue
  channel.declareQueue("FREC", AMQP::durable + AMQP::passive, arguments).onSuccess([&connection](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
    // report the name of the temporary queue
    std::cout << "Connecting to queue: " << name << std::endl;

    // now we can close the connection
    // connection.close();
  });
  // run the loop
  ev_run(loop, 0);

  // done
  return 0;
}

json processMessage(std::string message, int size)
{
  std::string res = message;
  res = res.substr(0, size);
  auto j3 = json::parse(res);

  std::string userID = j3["user_id"];
  std::string cadfileID = j3["cadfile_id"];
  std::string filename = j3["url"];

  if (false == fs::exists(filename))
  {
    json j;
    return j;
  }

  int startTime = clock();

  auto sheetMetalFeatureModel = std::make_shared<Fxt::SheetMetalComponent::SheetMetal>();
  auto loggingService = std::make_shared<Fxt::Logging::StandardOutputLogger>(userID);
  auto cadReaderFactory = std::make_shared<Fxt::CadFileReader::CadFileReaderFactory>(loggingService);

  auto cadFileReader = cadReaderFactory->createReader(filename.c_str());

  cadFileReader->extractFaces(sheetMetalFeatureModel, filename.c_str());
  sheetMetalFeatureModel->classifyFaces();
  sheetMetalFeatureModel->computeBendAngles();
  sheetMetalFeatureModel->assignBendDirection();

  if (sheetMetalFeatureModel->reduceModelSize())
  {
    loggingService->writeInfoEntry({"Done......."});
  }

  auto c_map = sheetMetalFeatureModel->getBends();

  std::vector<json> features;

  for (auto &[faceID, bend] : c_map)
  {
    std::cout << faceID << std::endl;

    json j2 = {
        {"bend_id", bend->getFaceId()},
        {"first_face_id", bend->getBendFeature()->getJoiningFaceID1()},
        {"second_face_id", bend->getBendFeature()->getJoiningFaceID2()},
        {"angle", bend->getBendFeature()->getBendAngle()},
        {"length", bend->getBendFeature()->getBendLength()},
        {"radius", bend->getBendFeature()->getBendRadius()},
        {"direction", bend->getBendFeature()->getBendDirection()}};

    features.push_back(j2);
  }

  int stopTime = clock();

  auto total_time = (stopTime - startTime) / double(CLOCKS_PER_SEC);

  j3["features"] = features;
  json jprops = {
      {"serial_data", save(sheetMetalFeatureModel)},
      {"thickness", sheetMetalFeatureModel->getThickness()},
      {"fre_time", total_time},
      {"bending_force", -1},
      {"process_level", ProcessLevel::FEATURE_EXTRACTED},
      {"bend_count", sheetMetalFeatureModel->getBends().size()}};

  j3["feature_props"] = jprops;

  // std::cout << "Received: " << j3.dump(4) << std::endl;

  loggingService->writeInfoEntry("{" + userID + "}: " + filename + ": Computations on the extracted features....");

  return j3;
}
