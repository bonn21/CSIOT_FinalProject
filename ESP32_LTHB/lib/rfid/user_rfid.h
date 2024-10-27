#ifndef _USER_RFID_H_
#define _USER_RFID_H_

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

void printDec(byte *buffer, byte bufferSize);
void printHex(byte *buffer, byte bufferSize);

extern int flag_open_door;

void RFID_Init();
void RFID_Check();


#endif