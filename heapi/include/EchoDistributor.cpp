#ifndef ECHO_DISTRIBUTOR_H
#define ECHO_DISTRIBUTOR_H

#include "AbstractDistributor.h"

template <typename T> class EchoDistributor : public AbstractDistributor {

protected:
  static virtual void handleResultResponse(Message m) override;
  static virtual void handleHealthResponse(Message m) override;
  static virtual void handleHardwareResponse(Message m) override;
  static virtual void handleAssistanceResponse(Message m) override;
  static virtual coap_response_t handleResponse(coap_session_t *session,
                                                const coap_pdu_t *sent,
                                                const coap_pdu_t *received,
                                                coap_mid_t id) override;

  virtual void observer(std::string uid, Matrix<T> a, Matrix<T> b,
                        MultiplicationMethod m) override;

public:
  Matrix<T> get(std::string id);

  std::string
  distributeMatrixMultiplication(Matrix<T> a, Matrix<T> b,
                                 MultiplicationMethod m = ROW_COLUMN);
  std::string distributeMatrixConvolution(Matrix<T> a, Matrix<T> b);

  void virtual waitOn(std::string id) override;
  void virtual configure(Parameter p, std::string value) override;
  void virtual configure(Parameter p, int value) override;
};

#endif // ECHO_DISTRIBUTOR_H
