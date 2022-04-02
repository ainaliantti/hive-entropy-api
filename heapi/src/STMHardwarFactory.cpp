#include "STMHardwareFactory.h"

Hardware STMHardwareFactory::create() {
  return Hardware(findRamSize(), findRamOccupation(), findProcessorFrequency(),
                  findProcessorCoreNumber(), findProcessorOccupation());
}

  float STMHardwareFactory::findRamSize() {
    return RAM_SIZE;
  }
  float STMHardwareFactory::findRamOccupation() {
    return RAM_OCCUPATION;
  }
  float STMHardwareFactory::findProcessorFrequency() {
    return PROCESSOR_FREQUENCY;
  }
  float STMHardwareFactory::findProcessorCoreNumber() {
    return CORE_NUMBER;
  }
  float STMHardwareFactory::findProcessorOccupation() {
    return RAM_OCCUPATION;
  }
