/*===================================================================================
 Из пульта управления передаются положение джойстика в двух плоскостях и
статусы нажатия четырех кнопок. Диапазоны величины отклонения положения
джойстика по каждой плоскости -100..100. Статусы кнопок передаются побитно.
Произвести проверку значений и вывести пользователю положение органов
управления при изменении их статуса
 ==================================================================================*/
#include <stdio.h>
#include <stdint.h> //c99, for constant length of int types
#include <stdlib.h>
//===================================================================================
//Prototypes of functions
unsigned short Crc16(unsigned char *pcBlock, unsigned short len);
void FillingData(int16_t address,int16_t *package_number,int8_t instruction,
		int8_t axis_x,int8_t axis_y,unsigned b0,unsigned b1,unsigned b2,unsigned b3);
extern int SendData(char *data);
void ReceiveData(char *data);
//===================================================================================
//Global variables
int8_t axis_x, axis_y;
unsigned b0,b1,b2,b3;
int16_t package_number=0;
//===================================================================================
unsigned short Crc16(unsigned char *pcBlock, unsigned short len){
/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021    x^16 + x^12 + x^5 + 1
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 байт (32767 бит) - обнаружение
    одинарных, двойных, тройных и всех нечетных ошибок
*/
    unsigned short crc = 0xFFFF;
    unsigned char i;
    while (len--)
    {
        crc ^= *pcBlock++ << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}
//===================================================================================
#pragma pack(push,1)
struct DataPackStruct
{						//2+2+1+1+1+1=8 BYTES (sizeof) WITHOUT CRC field!!!
	int16_t address;
	int16_t package_number;
	int8_t instruction;

	int8_t axis_x;
	int8_t axis_y;
	unsigned b0 :1;
	unsigned b1 :1;
	unsigned b2 :1;
	unsigned b3 :1;

	int16_t crc16;
} dataPack;
#pragma pack(pop)

struct DataPackStruct *dataP = &dataPack;

//===================================================================================
void FillingData(int16_t address,int16_t *pack_number,int8_t instruction,
		int8_t axis_x,int8_t axis_y,unsigned b0,unsigned b1,unsigned b2,unsigned b3){

dataP->address=address;
(dataP->package_number)= ++(*pack_number);
dataP->instruction=instruction;
dataP->axis_x=axis_x;
dataP->axis_y=axis_y;
dataP->b0=b0;
dataP->b1=b1;
dataP->b2=b2;
dataP->b3=b3;
dataP->crc16= Crc16(   ((unsigned char*)dataP), 8);
}
//===================================================================================
void ReceiveData(char *data){
	unsigned short a=Crc16(  (unsigned char*)data ,8 );
	unsigned short b=((struct DataPackStruct *)data)->crc16;

if ( a!=b ){
	printf("Package was lost! \r\n");

	printf("crc1 %hu \r\n",Crc16(  (unsigned char*)data ,8 ));
	printf("crc2 %hu \r\n", ((struct DataPackStruct *)data)->crc16   );
	printf("======================================");
}
else{
	struct DataPackStruct *myData=	(struct DataPackStruct *)data;
	if(   (axis_x==myData->axis_x)&(axis_y==myData->axis_y)&(b0==myData->b0)&(b1==myData->b1)&(b2==myData->b2)&(b3==myData->b3)  ){
		return;
	}
	else {
if (((myData->address)==9) &((myData->instruction)==0x01)) {
axis_x=myData->axis_x;
axis_y=myData->axis_y;
b0=myData->b0;
b1=myData->b1;
b2=myData->b2;
b3=myData->b3;

printf("x=%d, y=%d, b0=%d, b1=%d, b2=%d, b3=%d \r\n", axis_x, axis_y, b0,b1,b2,b3);
printf("======================================");
			}else
{printf("Wrong address and/or instructions");
			}
		}
	}
}
//===================================================================================
int main(void) {

printf("======================================\r\n");

FillingData(9, &package_number, 0x01, 10,23, 0,0,0,0);
SendData(((char*)dataP));
ReceiveData(((char*)dataP));
printf("%d \r\n",package_number);

FillingData(9, &package_number, 0x01, 11,23, 1,0,0,0);
SendData(((char*)dataP));
ReceiveData(((char*)dataP));
printf("%d \r\n",package_number);

FillingData(9, &package_number, 0x01, 11,23, 1,0,0,0);
SendData(((char*)dataP));
ReceiveData(((char*)dataP));
printf("%d \r\n",package_number);

FillingData(9, &package_number, 0x71, 11,29, 1,1,1,1);
SendData(((char*)dataP));
ReceiveData(((char*)dataP));
printf("%d \r\n",package_number);

FillingData(8, &package_number, 0x01, 19,23, 1,0,1,0);
SendData(((char*)dataP));
ReceiveData(((char*)dataP));
printf("%d \r\n",package_number);

	return EXIT_SUCCESS;
}
//===================================================================================
