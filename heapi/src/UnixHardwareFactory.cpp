#include "UnixHardwareFactory.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

Hardware UnixHardwareFactory::create() {
  ramSize = findRamSize();
  return Hardware(ramSize, findRamOccupation(), findProcessorFrequency(),
                  findProcessorCoreNumber(), findProcessorOccupation());
}

void execUnixCMD(const char *cmd, char *result) {
  FILE *fp = nullptr;
  char str[sizeof(float) + 1] = {0};

  fp = popen(cmd, "r");
  if (fp == nullptr) {
    printf("Failed to run command\n");
  }

  /* Read the output a line at a time - output it. */
  while (fgets(result, sizeof(float) * sizeof(float), fp) != nullptr) {
    std::cout << " fgets [" << str << "]" << std::endl;
  }
  std::cout << "for : " << cmd << " result : " << result << std::endl;
  /* close */
  pclose(fp);
}

float UnixHardwareFactory::findProcessorCoreNumber() {
  char socket[sizeof(float)] = {0}, core[sizeof(float)] = {0};

  execUnixCMD("nproc | tr -d '\n'", core);
  /* Open the command for reading. */

  float numberOfCore = std::stof(core);
  return numberOfCore;
}

float UnixHardwareFactory::findProcessorFrequency() {
  char numberOfLine[2] = {0}, frequency[10000] = {0};
  execUnixCMD(
      "lscpu | awk '$0 ~ /MHz/ {count ++} END {print count}' | tr -d '\n'",
      numberOfLine);
  std::cout << "number of line=" << numberOfLine << std::endl;

  if (std::strcmp(numberOfLine, "3") == 0) {
    execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '2p'",
                frequency);
  } else if (std::strcmp(numberOfLine, "2") == 0) {
    execUnixCMD("lscpu | grep MHz | sed -n '1p' | cut -d':' -f2 | tr -d ' '",
                frequency);
  } else if (std::strcmp(numberOfLine, "1") == 0) {
    execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '1p'",
                frequency);
  } else {
    std::cout << "wrong number of line=" << numberOfLine << std::endl;
  }
  std::cout << "frequency" << frequency << std::endl;
  float cpuFrequency = std::stof(frequency);
  return cpuFrequency;
}

float UnixHardwareFactory::findProcessorOccupation() {
  char occupation[10] = {0};
  execUnixCMD("iostat | sed -n '4p' | awk '{print $6}'", occupation);
  std::cout << "ProcessorOccupation=" << occupation << std::endl;
  std::string occupationRate_str = occupation;
  replace(occupationRate_str.begin(), occupationRate_str.end(), ',', '.');
  float cpuOccupation = 100 - stof(occupationRate_str);
  return cpuOccupation;
}

float UnixHardwareFactory::findRamSize() {
  char ram[10] = {0};
  execUnixCMD("free -m | grep Mem | awk '{print $2}'", ram);
  std::cout << "ram=" << ram;
  float ramSize = std::stof(ram);
  return ramSize;
}

float UnixHardwareFactory::findRamOccupation() {
  char usedRam[10] = {0};
  execUnixCMD("free -m | grep Mem | awk '{print $4}'", usedRam);
  std::cout << "usedRam=" << usedRam;
  return std::stof(usedRam) * 100 / ramSize;
}
