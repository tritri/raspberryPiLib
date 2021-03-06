#include "BME280.h"

void getRegisters(char address, int numData, unsigned char *data) {
  char adr;
  int i;

  adr = address;
  for (i = 0; i < numData; i++) {
    *data = wiringPiI2CReadReg8 (dev, adr);
    //      printf("adr:%x data:%x\n",adr, *data);
    adr ++;
    data ++;
  }
}

void readTrim()
{
  unsigned char data[32],i=0;

  getRegisters(0x88, 24, &data[0]);
  i+=24;

  getRegisters(0xA1, 1, &data[i]);
  i+=1;

  getRegisters(0xE1, 7, &data[i]);
  i+=7;

  dig_T1 = (data[1] << 8) | data[0];
  dig_T2 = (data[3] << 8) | data[2];
  dig_T3 = (data[5] << 8) | data[4];

  dig_P1 = (uint16_t)((data[7] << 8) | data[6]);
  dig_P2 = (int16_t)((data[9] << 8) | data[8]);
  dig_P3 = (int16_t)((data[11]<< 8) | data[10]);
  dig_P4 = (int16_t)((data[13]<< 8) | data[12]);
  dig_P5 = (int16_t)((data[15]<< 8) | data[14]);
  dig_P6 = (int16_t)((data[17]<< 8) | data[16]);
  dig_P7 = (int16_t)((data[19]<< 8) | data[18]);
  dig_P8 = (int16_t)((data[21]<< 8) | data[20]);
  dig_P9 = (int16_t)((data[23]<< 8) | data[22]);

  /*
  dig_P1 = (data[7] << 8) | data[6];
  dig_P2 = (data[9] << 8) | data[8];
  dig_P3 = (data[11]<< 8) | data[10];
  dig_P4 = (data[13]<< 8) | data[12];
  dig_P5 = (data[15]<< 8) | data[14];
  dig_P6 = (data[17]<< 8) | data[16];
  dig_P7 = (data[19]<< 8) | data[18];
  dig_P8 = (data[21]<< 8) | data[20];
  dig_P9 = (data[23]<< 8) | data[22];
  */
  
  dig_H1 = data[24];
  dig_H2 = (data[26]<< 8) | data[25];
  dig_H3 = data[27];
  dig_H4 = (data[28]<< 4) | (0x0F & data[29]);
  dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
  dig_H6 = data[31];
}

void writeReg(unsigned char address, unsigned char data) {
  char adr;
  int i;

  adr = address;
  wiringPiI2CWriteReg8 (dev, adr, data);
  //  printf("adr:%x data:%x\n",adr, data);
}

void readData()
{
  int i = 0;
  unsigned char data[8];

  getRegisters(0xF7, 8, &data[0]);

  pres_raw = data[0];
  pres_raw = (pres_raw<<8) | data[1];
  pres_raw = (pres_raw<<4) | (data[2] >> 4);

  temp_raw = data[3];
  temp_raw = (temp_raw<<8) | data[4];
  temp_raw = (temp_raw<<4) | (data[5] >> 4);

  hum_raw  = data[6];
  hum_raw  = (hum_raw << 8) | data[7];

  //printf("TEMP :%x  DegC  PRESS :%x  hPa  HUM :%x \n",temp_raw,pres_raw,hum_raw);
}


signed long int calibration_T(signed long int adc_T)
{

  signed long int var1, var2, T;
  var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;

  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}


unsigned long int calibration_P(signed long int adc_P)
{
  /*
	global  t_fine
	pressure = 0.0
	
	v1 = (t_fine / 2.0) - 64000.0
	v2 = (((v1 / 4.0) * (v1 / 4.0)) / 2048) * digP[5]
	v2 = v2 + ((v1 * digP[4]) * 2.0)
	v2 = (v2 / 4.0) + (digP[3] * 65536.0)
	v1 = (((digP[2] * (((v1 / 4.0) * (v1 / 4.0)) / 8192)) / 8)  + ((digP[1] * v1) / 2.0)) / 262144
	v1 = ((32768 + v1) * digP[0]) / 32768
	
	if v1 == 0:
		return 0
	pressure = ((1048576 - adc_P) - (v2 / 4096)) * 3125
	if pressure < 0x80000000:
		pressure = (pressure * 2.0) / v1
	else:
		pressure = (pressure / v1) * 2
	v1 = (digP[8] * (((pressure / 8.0) * (pressure / 8.0)) / 8192.0)) / 4096
	v2 = ((pressure / 4.0) * digP[7]) / 8192.0
	pressure = pressure + ((v1 + v2 + digP[6]) / 16.0)  

	print "pressure : %7.2f hPa" % (pressure/100)
  */
  
  double var1, var2;
  signed long int P;
  var1 = (t_fine / 2.0) - 64000;
  var2 = (((var1 / 4.0) * (var1 / 4.0)) / 2048) * dig_P6 ;
  var2 = var2 + ((var1 * dig_P5) * 2.0) ;
  var2 = (var2 / 4.0) + (dig_P4 * 65536.0);
  var1 = (((dig_P3 * (((var1 / 4.0) * (var1 / 4.0)) / 8192)) / 8)  + ((dig_P2 * var1) / 2.0)) / 262144;
  var1 = ((32768 + var1) * dig_P1) / 32768;
  if (var1 == 0)
    {
      return 0;
    }
  P = ((1048576 - adc_P) - (var2 / 4096)) * 3125;

  
  if(P<0x80000000)
    {
      P = (P * 2.0) / var1;
    }
  else
    {
      P = (P / var1) * 2;
    }
  
  var1 = (dig_P9 * (((P / 8.0) * (P / 8.0)) / 8192.0)) / 4096;
  var2 =((P / 4.0) * dig_P8) / 8192.0;
  P = P + ((var1 + var2 + dig_P7) / 16.0);
  return P;
}
/*
unsigned long int calibration_P(signed long int adc_P)
{
  signed long int var1, var2;
  unsigned long int P;
  var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
  var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
  var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
  var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
  var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
  var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
  if (var1 == 0)
    {
      return 0;
    }
  P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
  if(P<0x80000000)
    {
      P = (P << 1) / ((unsigned long int) var1);
    }
  else
    {
      P = (P / (unsigned long int)var1) * 2;
    }
  var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
  var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
  P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
  return P;
}
*/
unsigned long int calibration_H(signed long int adc_H)
{
  signed long int v_x1;

  v_x1 = (t_fine - ((signed long int)76800));
  v_x1 = (((((adc_H << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) +
	    ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) *
						    (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) *
						 ((signed long int) dig_H2) + 8192) >> 14));
  v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
  v_x1 = (v_x1 < 0 ? 0 : v_x1);
  v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
  return (unsigned long int)(v_x1 >> 12);
}

int init_dev(void)
{
  unsigned char osrs_t = 1;             //Temperature oversampling x 1
  unsigned char osrs_p = 1;             //Pressure oversampling x 1
  unsigned char osrs_h = 1;             //Humidity oversampling x 1
  unsigned char mode = 3;               //Normal mode
  unsigned char t_sb = 5;               //Tstandby 1000ms
  unsigned char filter = 0;             //Filter off
  unsigned char spi3w_en = 0;           //3-wire SPI Disable

  unsigned char ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
  unsigned char config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
  unsigned char ctrl_hum_reg  = osrs_h;


  double temp_act = 0.0, press_act = 0.0,hum_act=0.0;
  signed long int temp_cal;
  unsigned long int press_cal,hum_cal;

  if ((dev = wiringPiI2CSetup(BME280_ADDRESS)) == -1)
    {
      return 1 ;
    }
  writeReg(0xF2,ctrl_hum_reg);
  writeReg(0xF4,ctrl_meas_reg);
  writeReg(0xF5,config_reg);
  readTrim();

  return 0;
}
