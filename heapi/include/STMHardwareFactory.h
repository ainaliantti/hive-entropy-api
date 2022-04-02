#ifndef STM_HARDWARE_FACTORY_H
#define STM_HARDWARE_FACTORY_H

#include "IHardwareFactory.h"

#ifdef STM32F746xx
#include "stm32f7xx_hal_conf.h"
#define PROCESSOR_FREQUENCY 216000000 // 216 MHz, Could be possibly retrieved from conf header.
#define PROCESSOR_OCCUPATION 0
#define CORE_NUMBER 1
#define RAM_SIZE 0x4e200 // 320 KB
#define RAM_OCCUPATION 0.30 // reserve 30% for system and main task

#else // Unknown system
#define PROCESSOR_FREQUENCY 0
#define PROCESSOR_OCCUPATION 0
#define CORE_NUMBER 0
#define RAM_SIZE 0
#define RAM_OCCUPATION 0

#endif // STM32F746xx

/**
 * @brief Implementation of hardware factory for various STM development boards.
 */
class STMHardwareFactory: public IHardwareFactory {

protected:
  float findRamSize() override;
  float findRamOccupation() override;
  float findProcessorFrequency() override;
  float findProcessorCoreNumber() override;
  float findProcessorOccupation() override;

public:
  Hardware create() override;

};


#endif // STM_HARDWARE_FACTORY_H