#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>

/**
 * @brief Contains the hardware specification of a node.
 */
class Hardware {

private:
  /**
   * @brief Size of node's RAM
   */
  float ramSize;

  /**
   * @brief Occupation rate of node's RAM between 0 and 100
   */
  float ramOccupation;

  /**
   * @brief Precessor's frequency in MHz
   */
  float processorFrequency;

  /**
   * @brief Number of logical core
   */
  float processorCoreNumber;

  /**
   * @brief Occupation rate of CPU between 0 and 100
   */
  float processorOccupation;

public:
  explicit Hardware(std::string infos);

  /**
   * @brief Copy constructor for Hardware.
   *
   * @param other
   */
  Hardware(const Hardware &other) = default;

  Hardware(const float ramSize, const float ramOccupation,
           const float processorFrequency, const float processorCoreNumber,
           const float processorOccupation);

  ~Hardware() = default;

  std::string toString() const;

  float getRamSize();
  float getRamOccupation();
  float getProcessorFrequency();
  float getProcessorCoreNumber();
  float getProcessorOccupation();
};

#endif // HARDWARE_H
