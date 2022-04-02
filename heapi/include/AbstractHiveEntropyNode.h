#ifndef ABSTRACT_HIVE_ENTROPY_NODE_H
#define ABSTRACT_HIVE_ENTROPY_NODE_H

#include "AbstractCoapEndpoint.h"

class AbstractHiveEntropyNode {
protected:
  AbstractCoapEndpoint coap;

public:
  void send(Message const &m);

  void sendHardwareSpecification(std::string uri);
  void sendAskingHardwareSpecification(std::string uri);
  void checkLiveliness(std::string target);
  void queryNodeAvailability();
  void resolveNodeIdentities();

  void registerResponseHandler(coap_response_handler_t const &func);

  template <Message (*F)(Message)>
  void registerMessageHandler(std::string const &uri, HttpMethod const &method);

  template <Message (*F)(Message)>
  void registerAsynchronousHandler(std::string const &uri,
                                   HttpMethod const &method);

  void keepAlive();
};


#endif // ABSTRACT_HIVE_ENTROPY_NODE_H