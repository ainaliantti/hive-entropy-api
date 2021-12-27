#include "Message.h"

#include <arpa/inet.h>
#include <iostream>
#include <spdlog/spdlog.h>

Message::Message() { headers = std::map<Headers, std::string>(); }

Message::Message(coap_session_t const *sess, const coap_pdu_t *pdu) {
  // TYPE
  switch (coap_pdu_get_type(pdu)) {
  case COAP_MESSAGE_CON:
    type = MessageType::CONFIRMABLE;
    break;
  case COAP_MESSAGE_NON:
    type = MessageType::NON_CONFIRMABLE;
    break;
  case COAP_MESSAGE_ACK:
    type = MessageType::ACK;
    break;
  case COAP_MESSAGE_RST:
    type = MessageType::RESET;
    break;
  }

  // METHOD
  switch (coap_pdu_get_code(pdu)) {
  case COAP_REQUEST_CODE_GET:
    httpMethod = HttpMethod::GET;
    break;
  case COAP_REQUEST_CODE_POST:
    httpMethod = HttpMethod::POST;
    break;
  case COAP_REQUEST_CODE_PUT:
    httpMethod = HttpMethod::PUT;
    break;
  case COAP_REQUEST_CODE_DELETE:
    httpMethod = HttpMethod::DELETE;
    break;
  case COAP_RESPONSE_CODE_OK:
    httpMethod = HttpMethod::OK;
    break;
  case COAP_RESPONSE_CODE_NOT_FOUND:
    httpMethod = HttpMethod::NOT_FOUND;
    break;
  case COAP_RESPONSE_CODE_BAD_REQUEST:
    httpMethod = HttpMethod::BAD_REQUEST;
    break;
  default:
    spdlog::warn("The PDU {} is not handled properly.", coap_pdu_get_code(pdu));
  }

  // OPTIONS
  coap_opt_iterator_t iter;
  coap_option_iterator_init(pdu, &iter, COAP_OPT_ALL);

  headers = std::map<Headers, std::string>();

  coap_opt_t *current = coap_option_next(&iter);
  while (current != nullptr) {

    std::string optVal(reinterpret_cast<const char *>(coap_opt_value(current)),
                       coap_opt_length(current));

    switch (iter.number) {
    case 2048:
      headers.insert(std::pair<Headers, std::string>(Headers::PURPOSE, optVal));
      break;
    case 2050:
      headers.insert(
          std::pair<Headers, std::string>(Headers::CALCULATION_ID, optVal));
      break;
    case 2052:
      headers.insert(std::pair<Headers, std::string>(Headers::TASK_ID, optVal));
      break;
    case 2054:
      headers.insert(
          std::pair<Headers, std::string>(Headers::INSERT_AT_X, optVal));
      break;
    case 2056:
      headers.insert(
          std::pair<Headers, std::string>(Headers::INSERT_AT_Y, optVal));
      break;
    case 2058:
      headers.insert(
          std::pair<Headers, std::string>(Headers::BORDER_SIZE, optVal));
      break;
    case 2060:
      headers.insert(std::pair<Headers, std::string>(Headers::STEPS, optVal));
      break;
    case 2062:
      headers.insert(
          std::pair<Headers, std::string>(Headers::ELEMENT_TYPE, optVal));
      break;
    case 2064:
      headers.insert(
          std::pair<Headers, std::string>(Headers::ENCODING_TYPE, optVal));
      break;
    case 2066:
      headers.insert(
          std::pair<Headers, std::string>(Headers::SERIALIZED_TYPE, optVal));
      break;
    case 2068:
      headers.insert(std::pair<Headers, std::string>(
          Headers::ASSISTANCE_RESPONSE, optVal));
      break;
    case COAP_OPTION_PROXY_URI:
      headers.insert(
          std::pair<Headers, std::string>(Headers::PROXY_URI, optVal));
      break;
    }

    current = coap_option_next(&iter);
  }

  // CONTENT

  size_t bodySize;
  const uint8_t *data;

  if (coap_get_data(pdu, &bodySize, &data)) {
    std::string tempContent(reinterpret_cast<const char *>(data), bodySize);
    if (tempContent.length() > 0) {
      content = tempContent;
    }
  }

  // DEST
  char dest_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(coap_session_get_addr_local(sess)->addr.sin.sin_addr),
            dest_buf, INET_ADDRSTRLEN);
  std::string tempHost(dest_buf);
  dest = tempHost + ":" +
         std::to_string(
             htons(coap_session_get_addr_local(sess)->addr.sin.sin_port));

  // PEER
  char peer_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(coap_session_get_addr_remote(sess)->addr.sin.sin_addr),
            peer_buf, INET_ADDRSTRLEN);
  std::string tempPeerHost(peer_buf);
  peer = tempPeerHost + ":" +
         std::to_string(
             htons(coap_session_get_addr_remote(sess)->addr.sin.sin_port));
}

std::map<Headers, std::string> Message::getHeaders() const { return headers; }

std::string Message::getDest() const { return dest; }

MessageType Message::getType() const { return type; }

HttpMethod Message::getHttpMethod() const { return httpMethod; }

std::string Message::getContent() const { return content; }

std::string Message::getPeer() const { return peer; }

void Message::addHeader(Headers const &key, std::string const &val) {
  headers.insert(std::pair<Headers, std::string>(key, val));
}

void Message::setContent(std::string const &content) {
  this->content = content;
}

void Message::setDest(std::string const &dest) { this->dest = dest; }

void Message::setHttpMethod(HttpMethod const &m) { this->httpMethod = m; }

void Message::setType(MessageType const &type) { this->type = type; }

coap_pdu_t *Message::toCoapMessage(coap_session_t *sess) {
  coap_pdu_type_t coapType;
  coap_pdu_code_t coapMethod;

  switch (type) {
  case MessageType::CONFIRMABLE:
    coapType = COAP_MESSAGE_CON;
    break;
  case MessageType::NON_CONFIRMABLE:
    coapType = COAP_MESSAGE_NON;
    break;
  case MessageType::RESET:
  case MessageType::ACK:
    coapType = COAP_MESSAGE_ACK;
    break;
  }

  switch (httpMethod) {
  case HttpMethod::GET:
    coapMethod = COAP_REQUEST_CODE_GET;
    break;
  case HttpMethod::POST:
    coapMethod = COAP_REQUEST_CODE_POST;
    break;
  case HttpMethod::PUT:
    coapMethod = COAP_REQUEST_CODE_PUT;
    break;
  case HttpMethod::DELETE:
    coapMethod = COAP_REQUEST_CODE_DELETE;
    break;
  case HttpMethod::OK:
    coapMethod = COAP_RESPONSE_CODE_OK;
    break;
  case HttpMethod::NOT_FOUND:
    coapMethod = COAP_RESPONSE_CODE_NOT_FOUND;
    break;
  case HttpMethod::BAD_REQUEST:
    coapMethod = COAP_RESPONSE_CODE_BAD_REQUEST;
    break;
  }

  coap_optlist_t *optlist_chain = nullptr;

  coap_pdu_t *transformed = coap_new_pdu(coapType, coapMethod, sess);

  //====TOKEN====
  size_t tokenLength;
  uint8_t tokenBuffer[1024];
  coap_session_new_token(sess, &tokenLength, tokenBuffer);
  if (coap_add_token(transformed, tokenLength, tokenBuffer) == 0)
    throw "Unable to add token to the message";

  //====URI====
  coap_uri_t uri_path;
  if (coap_split_uri(reinterpret_cast<const uint8_t *>(dest.c_str()),
                     dest.length(), &uri_path)) {
    std::cout << "Error splitting the URI" << std::endl;
  }

  std::string path(reinterpret_cast<const char *>(uri_path.path.s),
                   uri_path.path.length);
  int delimiterPosition;

  while ((delimiterPosition = static_cast<int>(path.find('/'))) !=
         static_cast<int>(std::string::npos)) {
    std::string token(path.substr(0, delimiterPosition));
    if (!coap_insert_optlist(
            &optlist_chain,
            coap_new_optlist(COAP_OPTION_URI_PATH, token.length(),
                             reinterpret_cast<const uint8_t *>(token.c_str()))))
      throw "Couldn't insert URI option";
    path.erase(0, delimiterPosition + 1);
  }

  if (!path.empty())
    if (!coap_insert_optlist(
            &optlist_chain,
            coap_new_optlist(COAP_OPTION_URI_PATH, path.length(),
                             reinterpret_cast<const uint8_t *>(path.c_str()))))
      throw "Couldn't insert URI option";

  //====ADD OPTIONS====
  for (std::pair<Headers, std::string> el : headers) {
    uint16_t opt_num;

    switch (el.first) {
    case Headers::PURPOSE:
      opt_num = 2048;
      break;
    case Headers::CALCULATION_ID:
      opt_num = 2050;
      break;
    case Headers::TASK_ID:
      opt_num = 2052;
      break;
    case Headers::INSERT_AT_X:
      opt_num = 2054;
      break;
    case Headers::INSERT_AT_Y:
      opt_num = 2056;
      break;
    case Headers::BORDER_SIZE:
      opt_num = 2058;
      break;
    case Headers::STEPS:
      opt_num = 2060;
      break;
    case Headers::ELEMENT_TYPE:
      opt_num = 2062;
      break;
    case Headers::ENCODING_TYPE:
      opt_num = 2064;
      break;
    case Headers::SERIALIZED_TYPE:
      opt_num = 2066;
      break;
    case Headers::ASSISTANCE_RESPONSE:
      opt_num = 2068;
      break;
    case Headers::PROXY_URI:
      opt_num = COAP_OPTION_PROXY_URI;
      break;
    }

    if (!coap_insert_optlist(&optlist_chain,
                             coap_new_optlist(opt_num, el.second.length(),
                                              reinterpret_cast<const uint8_t *>(
                                                  el.second.c_str()))))
      throw "Couldn't insert passed option";
  }

  if (!coap_add_optlist_pdu(transformed, &optlist_chain))
    throw "Could not add option list";

  //====CONTENT DECLARATION====
  if (!content.empty())
    if (coap_add_data_large_request(
            sess, transformed, content.length(),
            reinterpret_cast<const uint8_t *>(content.c_str()), NULL,
            NULL) == 0)
      throw "Unable to add content to the message";

  return transformed;
}

void Message::fillResponse(coap_resource_t *resource, coap_session_t *sess,
                           coap_pdu_t const *request, coap_pdu_t *response) {

  coap_pdu_type_t messageType;

  switch (type) {
  case MessageType::CONFIRMABLE:
    messageType = COAP_MESSAGE_CON;
    break;
  case MessageType::NON_CONFIRMABLE:
    messageType = COAP_MESSAGE_NON;
    break;
  case MessageType::ACK:
    messageType = COAP_MESSAGE_ACK;
    break;
  case MessageType::RESET:
    messageType = COAP_MESSAGE_RST;
    break;
  default:
    messageType = COAP_MESSAGE_ACK;
  }

  coap_pdu_set_type(response, messageType);

  coap_pdu_code_t coapMethod;

  switch (httpMethod) {
  case HttpMethod::GET:
    coapMethod = COAP_REQUEST_CODE_GET;
    break;
  case HttpMethod::POST:
    coapMethod = COAP_REQUEST_CODE_POST;
    break;
  case HttpMethod::PUT:
    coapMethod = COAP_REQUEST_CODE_PUT;
    break;
  case HttpMethod::DELETE:
    coapMethod = COAP_REQUEST_CODE_DELETE;
    break;
  case HttpMethod::OK:
    coapMethod = COAP_RESPONSE_CODE_OK;
    break;
  case HttpMethod::NOT_FOUND:
    coapMethod = COAP_RESPONSE_CODE_NOT_FOUND;
    break;
  case HttpMethod::BAD_REQUEST:
    coapMethod = COAP_RESPONSE_CODE_BAD_REQUEST;
    break;
  }
  coap_pdu_set_code(response, coapMethod);

  if (!headers.empty()) {
    coap_optlist_t *chain = nullptr;

    for (std::pair<Headers, std::string> el : headers) {
      uint16_t opt_num;
      switch (el.first) {
      case Headers::PURPOSE:
        opt_num = 2048;
        break;
      case Headers::CALCULATION_ID:
        opt_num = 2050;
        break;
      case Headers::TASK_ID:
        opt_num = 2052;
        break;
      case Headers::INSERT_AT_X:
        opt_num = 2054;
        break;
      case Headers::INSERT_AT_Y:
        opt_num = 2056;
        break;
      case Headers::BORDER_SIZE:
        opt_num = 2058;
        break;
      case Headers::STEPS:
        opt_num = 2060;
        break;
      case Headers::ELEMENT_TYPE:
        opt_num = 2062;
        break;
      case Headers::ENCODING_TYPE:
        opt_num = 2064;
        break;
      case Headers::SERIALIZED_TYPE:
        opt_num = 2066;
        break;
      case Headers::ASSISTANCE_RESPONSE:
        opt_num = 2068;
        break;
      case Headers::PROXY_URI:
        opt_num = COAP_OPTION_PROXY_URI;
        break;
      }

      if (!coap_insert_optlist(
              &chain, coap_new_optlist(opt_num, el.second.length(),
                                       reinterpret_cast<const uint8_t *>(
                                           el.second.c_str()))))
        throw "Couldn't insert passed option";
    }

    if (!coap_add_optlist_pdu(response, &chain))
      throw "Could not add option list";
  }

  //====ADD CONTENT====
  if (!content.empty())
    if (coap_add_data_large_response(
            resource, sess, request, response, nullptr, COAP_MEDIATYPE_ANY, -1,
            0, content.length(),
            reinterpret_cast<const uint8_t *>(content.c_str()), nullptr,
            nullptr) == 0)
      throw "Unable to add content to the message";
}
