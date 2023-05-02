#ifndef _CLI_H_
#define _CLI_H_

#include "Arduino.h"

#define RX_PIN          3
#define CLI_BUF_SIZE    128

void initCLI(uint32_t baudrate);
void serialISR(void);
void processInput(void* param);
void printBootMessage(void);
void printCLIHead(void);

#endif