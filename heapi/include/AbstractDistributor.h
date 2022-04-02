#ifndef ABSTRACT_DISTRIBUTOR_H
#define ABSTRACT_DISTRIBUTOR_H

#include <coap3/coap.h>
#include <memory>
#include <mutex>
#include <set>
#include <string>

enum Parameter {
  ASSISTANCE_TIMEOUT,
  ASSISTANCE_MAX_PARTICIPANTS,
  UID_SEED,
  MAX_THREADS,
  FRESHNESS,
  RESULT_TIMEOUT,
  HEALTH_TIMEOUT
};

class Peer;
class Message;
class HiveEntropyNode;

/**
 * @brief Abstract base class for distributors. Used for handling the distribution of calculation
 * to HiveEntropy nodes
 */
template <typename T> class AbstractDistributor {
protected:
  std::shared_ptr<HiveEntropyNode> node;

  static std::map<Parameter, std::string> settings;
  static std::vector<Peer> peers;

  static std::chrono::steady_clock::time_point lastHardwareCheck;
  static std::vector<Peer> healthyNodes;

  bool destructionAsked;

  static virtual void handleResultResponse(Message m) = 0;
  static virtual void handleHealthResponse(Message m) = 0;
  static virtual void handleHardwareResponse(Message m) = 0;
  static virtual void handleAssistanceResponse(Message m) = 0;
  static virtual coap_response_t handleResponse(coap_session_t *session,
                                                const coap_pdu_t *sent,
                                                const coap_pdu_t *received,
                                                coap_mid_t id) = 0;

  virtual void observer(std::string uid, Matrix<T> a, Matrix<T> b,
                        MultiplicationMethod m) = 0;

  std::string generateUID();

public:
  // Constructors
  explicit AbstractDistributor(std::shared_ptr<HiveEntropyNode> const &n);
  ~AbstractDistributor();

  void virtual waitOn(std::string id) = 0;
  void virtual configure(Parameter p, std::string value) = 0;
  void virtual configure(Parameter p, int value) = 0;
};

#endif // ABSTRACT_DISTRIBUTOR_H