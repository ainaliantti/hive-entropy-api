#include "AbstractDistributor.h"
#include "Block.h"
#include "Hardware.h"
#include "HiveEntropyNode.h"
#include "Peer.h"
#include <chrono>
#include <utility>

using namespace std::chrono_literals;

/*/////////////////////////////////////
//  Template explicit instanciation  //
/////////////////////////////////////*/
template class AbstractDistributor<int>;

template class AbstractDistributor<float>;

template class AbstractDistributor<double>;

template class AbstractDistributor<unsigned short>;

template class AbstractDistributor<unsigned char>;

template <typename T>
AbstractDistributor<T>::AbstractDistributor(
    std::shared_ptr<HiveEntropyNode> const &n)
    : node(n) {
  node->registerResponseHandler(handleResponse);
  configure(Parameter::ASSISTANCE_TIMEOUT, 5);
  configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS, 20);
  configure(Parameter::RESULT_TIMEOUT, 10);
  configure(Parameter::HEALTH_TIMEOUT, 2);
  configure(Parameter::MAX_THREADS, 4);
  configure(Parameter::FRESHNESS, 10);
#ifdef SPDLOG_API
  spdlog::set_level(spdlog::level::info);
  spdlog::set_pattern("[%H:%M:%S.%e] [%!] (%l) %v");
#endif
  /*Peer p;
  p.setAddress("192.168.1.35:9999");
  peers.push_back(p);*/
}

template <typename T> AbstractDistributor<T>::~AbstractDistributor() {
  std::unique_lock<std::mutex> lock(destructionLock);
  destructionAsked = true;
}

template <typename T>
void AbstractDistributor<T>::configure(Parameter p, std::string value) {
  settings[p] = value;
}

template <typename T>
void AbstractDistributor<T>::configure(Parameter p, int value) {
  settings[p] = std::to_string(value);
}

template <typename T>
coap_response_t AbstractDistributor<T>::handleResponse(
    coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received,
    coap_mid_t id) {
  Message m(session, received);
  if (m.getHeaders().find(Headers::PURPOSE) != m.getHeaders().end()) {
    std::string purpose = m.getHeaders()[Headers::PURPOSE];
    // spdlog::info("Received message with {} purpose",purpose);
    if (purpose == PURPOSE_ASSISTANCE) {
      handleAssistanceResponse(m);
    } else if (purpose == PURPOSE_HARDWARE) {
      handleHardwareResponse(m);
    } else if (purpose == PURPOSE_RESULT) {
      handleResultResponse(m);
    } else if (purpose == PURPOSE_HEALTH) {
      handleHealthResponse(m);
    } else {
      // spdlog::warn("Unrecognized purpose {}. Skipping handling...",purpose);
    }
  }

  return COAP_RESPONSE_OK;
}

template <typename T>
void AbstractDistributor<T>::handleHardwareResponse(Message m) {
  std::string address = m.getPeer();
  Hardware hw(m.getContent());
#ifdef SPDLOG_API
  spdlog::info("Received hardware specification {}", hw.toString());
#endif
  std::chrono::steady_clock::duration delay =
      std::chrono::steady_clock::now() - lastHardwareCheck;

  Peer peer(hw, address, delay);
  peers.push_back(peer);
}

template <typename T> std::string AbstractDistributor<T>::generateUID() {
  if (settings.find(Parameter::UID_SEED) != settings.end()) {
    uint64_t seed[] = {1};
    char *seedTable = reinterpret_cast<char *>(seed);
    settings[Parameter::UID_SEED] = std::string(seedTable, 8);
  }

  char currUIDStr[settings[Parameter::UID_SEED].size()];
  strncpy(currUIDStr, settings[Parameter::UID_SEED].c_str(),
          settings[Parameter::UID_SEED].size());

  uint64_t *currUID = reinterpret_cast<uint64_t *>(currUIDStr);
  (*currUID) += 1;
  settings[Parameter::UID_SEED] =
      std::string(reinterpret_cast<char *>(currUID), 8);
  return settings[Parameter::UID_SEED];
}