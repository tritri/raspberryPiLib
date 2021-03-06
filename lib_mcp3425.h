#ifndef LIB_MCP3425
#define LIB_MCP3425

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

//MPC3425コンフィギュレーション
#define CONFIG 0x88
#define MPL3425_ID 0x68

//温度測定用サーミスタ設定
#define stdVoltage 3.3 //電源電圧
#define shuntReg 47000 //シャント抵抗値
#define B 3380 //サーミスタB定数
#define Tr25 10000 //25℃サーミスタ抵抗値

double calcVoltage(int);
double getTemperature(int);

#endif
