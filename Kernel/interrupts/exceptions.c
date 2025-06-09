#include <exceptions.h>

static void zero_division();
static void invalid_operation_code();
void printRegisters();
extern uint64_t register_array[];
extern void load_registers_array();
char *regs_strings[] = {
    " RAX: ", " RBX: ", " RCX: ", " RDX: ", " RSI: ", " RDI: ",
    " RBP: ", " RSP: ", " R8: ",  " R9: ",  " R10: ", " R11: ",
    " R12: ", " R13: ", " R14: ", " R15: ", " RIP: "};

void exceptionDispatcher(int exception) {
  switch (exception) {
  case ZERO_EXCEPTION_ID:
    zero_division();
    break;
  case INVALID_OPERATION_CODE_ID:
    invalid_operation_code();
    break;
  }
  print("\n", 1, STDOUT);
  printRegisters();
}

void printRegisters() {
  for (int i = 0; i < CANT_REGS; i++) {
    print(regs_strings[i], str_len(regs_strings[i]), STDOUT);
    uint64_t value = register_array[i];
    char hex_str[19];
    hex_str[0] = '0';
    hex_str[1] = 'x';
    for (int j = 0; j < 16; j++) {
      int shift = (15 - j) * 4;
      uint8_t digit = (value >> shift) & 0xF;
      hex_str[2 + j] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
    }
    hex_str[18] = '\0';
    print(hex_str, 18, STDOUT);
    print("\n", 1, STDOUT);
  }
}

static void zero_division() { printStd("ZERO DIVISION ERROR", 19); }

static void invalid_operation_code() { printStd("OPCODE ERROR", 12); }
