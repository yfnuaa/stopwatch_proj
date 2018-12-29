#ifndef __EEPROM__H_
#define __EEPROM__H_
char IapRead(int addr);
void IapWrite(int addr,char dat);
 
#endif