#ifndef UNIX_HARDWARE_FACTORY_H
#define UNIX_HARDWARE_FACTORY_H

#include "IHardwareFactory.h"

/**
 * @brief Implementation of hardware factory for *Nix systems.
 */
class UnixHardwareFactory : public IHardwareFactory {
private:
  float ramSize;

protected:
  float findRamSize() override;
  float findRamOccupation() override;
  float findProcessorFrequency() override;
  float findProcessorCoreNumber() override;
  float findProcessorOccupation() override;

public:
  Hardware create() override;
};

#endif // UNIX_HARDWARE_FACTORY_H
