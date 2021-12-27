#ifndef MATRIX_DISTRIBUTOR_H
#define MATRIX_DISTRIBUTOR_H

#include "AbstractDistributor.h"

#include <bits/stdc++.h>
#include <coap3/coap.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <spdlog/spdlog.h>
#include <string>

enum MultiplicationMethod { CANNON, ROW_COLUMN, MULTIPLE_ROW_COLUMN };

template <typename T> class Matrix;
template <typename T> class MatrixDistributor : public AbstractDistributor {
private:
  static std::map<std::string, std::mutex> locks; // Cleanup
  static std::mutex addressLock;

  static std::map<std::string, std::condition_variable> cvs; // Cleanup
  static std::condition_variable addressCv;

  static std::map<std::string, std::vector<Block>> pendingBlocks; // Cleanup

  static std::map<std::string, Matrix<T>> storedPartialResults; // Cleanup

  std::mutex destructionLock;

  void splitMatrixMultiplicationTask(std::string uid, Matrix<T> a, Matrix<T> b,
                                     MultiplicationMethod m);
  void splitMatrixConvolutionTask(std::string uid, Matrix<T> a, Matrix<T> b);

protected:
  std::shared_ptr<HiveEntropyNode> node;

  static std::map<Parameter, std::string> settings;
  static std::vector<Peer> peers;

  static std::chrono::steady_clock::time_point lastHardwareCheck;
  static std::vector<Peer> healthyNodes;

  bool destructionAsked;

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

  std::string generateUID();

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

#endif