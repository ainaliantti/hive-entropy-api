#include "Hardware.h"

Hardware::Hardware(const float ramSize, const float ramOccupation,
                   const float processorFrequency,
                   const float processorCoreNumber,
                   const float processorOccupation)
    : ramSize(ramSize), ramOccupation(ramOccupation),
      processorFrequency(processorFrequency),
      processorCoreNumber(processorCoreNumber),
      processorOccupation(processorOccupation) {}

Hardware::Hardware(std::string infos) {
  size_t pos = 0;
  std::string delimiter = ":";
  std::string token;
  float infosList[5];
  for (float &i : infosList) {
    pos = infos.find(delimiter);
    token = infos.substr(0, pos);
    i = std::stof(token);
    infos.erase(0, pos + delimiter.length());
  }
  processorCoreNumber = infosList[0];
  processorFrequency = infosList[1];
  processorOccupation = infosList[2];
  ramSize = infosList[3];
  ramOccupation = infosList[4];
}

std::string Hardware::toString() const {
  std::string infos = std::to_string(processorCoreNumber) + ":" +
                      std::to_string(processorFrequency) + ':' +
                      std::to_string(processorOccupation) + ':' +
                      std::to_string(ramSize) + ':' +
                      std::to_string(ramOccupation);
  return infos;
}

float Hardware::getProcessorCoreNumber() { return processorCoreNumber; }

float Hardware::getProcessorFrequency() { return processorFrequency; }

float Hardware::getProcessorOccupation() { return processorOccupation; }

float Hardware::getRamSize() { return ramSize; }

float Hardware::getRamOccupation() { return ramOccupation; }
