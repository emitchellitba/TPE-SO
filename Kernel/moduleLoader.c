// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <stdint.h>

static void loadModule(uint8_t **module, void *targetModuleAddress);
static uint32_t readUint32(uint8_t **address);

void loadModules(void *payloadStart, void **targetModuleAddress) {
  int i;
  uint8_t *currentModule = (uint8_t *)payloadStart;
  uint32_t moduleCount = readUint32(&currentModule);

  for (i = 0; i < moduleCount; i++)
    loadModule(&currentModule, targetModuleAddress[i]);
}

static void loadModule(uint8_t **module, void *targetModuleAddress) {
  uint32_t moduleSize = readUint32(module);

  memcpy(targetModuleAddress, *module, moduleSize);
  *module += moduleSize;
}

static uint32_t readUint32(uint8_t **address) {
  uint32_t result = *(uint32_t *)(*address);
  *address += sizeof(uint32_t);
  return result;
}
