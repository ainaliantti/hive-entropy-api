#ifndef I_HARDWARE_FACTORY
#define I_HARDWARE_FACTORY

#include "Hardware.h"

/**
 * @brief Interface for creating Hardware objects
 */
class IHardwareFactory {

protected:
  virtual float findRamSize() = 0;
  virtual float findRamOccupation() = 0;
  virtual float findProcessorFrequency() = 0;
  virtual float findProcessorCoreNumber() = 0;
  virtual float findProcessorOccupation() = 0;

public:
  virtual Hardware create() = 0;
};

#endif // I_HARDWARE_FACTORY