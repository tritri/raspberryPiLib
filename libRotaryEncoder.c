#include "libRotaryEncoder.h"

int getPosition(){
  int n;

  int dataA = digitalRead(ROTARY_PORTA);
  int dataB = digitalRead(ROTARY_PORTB);
  //printf("\rGPIO%d = %d GPIO%d = %d",ROTARY_PORTA,dataA,ROTARY_PORTB,dataB);
 
  if( dataCurrentA!=dataA || dataCurrentB!=dataB){
    //printf("\rGPIO%d = %d GPIO%d = %d",ROTARY_PORTA,dataA,ROTARY_PORTB,dataB);
    indexTableDirection = (indexTableDirection << 2) + ((dataA<<1)+dataB);   /* 前回値と今回値でインデックスとする */
    n = dir[indexTableDirection & 15];                    /* 変化パターンから動きを得る */
    if (n) {                            /* 動きがあったら位置更新 */
      position += n;
    }
    //printf("moved = %d\n",position);
    //printf("GPIO%d = %d\n",ROTARY_PORTA,dataA);
    //printf("GPIO%d = %d\n",ROTARY_PORTB,dataB);
     
    dataCurrentA=dataA;
    dataCurrentB=dataB;


  }
  return position;
}

void getPositionISR(){
  int n;
  int indexTableDirectionCurrent;
  //まず値を取得します
  int dataABefore = digitalRead(ROTARY_PORTA);
  int dataBBefore = digitalRead(ROTARY_PORTB);
  int indexTableDirectionBefore  = (indexTableDirectionCurrent << 2) + ((dataABefore<<1)+dataBBefore);

  //チャタリング防止のため少し待つ
  delay(50);//20ms待ち
  
  int dataA = digitalRead(ROTARY_PORTA);
  int dataB = digitalRead(ROTARY_PORTB);

  
  if(indexTableDirectionCurrent != indexTableDirectionBefore){  
    //if( dataABefore!=dataA || dataBBefore!=dataB){
    indexTableDirectionCurrent = (indexTableDirectionBefore << 2) + ((dataA<<1)+dataB);   /* 前回値と今回値でインデックスとする */
    n = dir[indexTableDirectionCurrent & 15];                    /* 変化パターンから動きを得る */
    if (n) {                            /* 動きがあったら位置更新 */
      positionRotary += n;
    }
  }
  //printf("Now Position %d\n",position);
}
