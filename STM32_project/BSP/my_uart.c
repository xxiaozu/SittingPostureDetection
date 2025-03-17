
#include <stdio.h>
#include "my_uart.h"
#include "string.h"

uint8_t aRxBuf_3, DistanceBuff[10] = {0};
uint8_t Receive_OK = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t i = 0, rebuf[10] = {0};
	if(huart == (&huart3))
	{
		rebuf[i++] = aRxBuf_3;//�����������
		if(rebuf[0] != 0x5a)//�ж��Ƿ�Ϊ������ʼ
			i = 0;
		if((i == 2) && (rebuf[1] != 0x5a))//�ж����ݸ�ʽ��ȷ
			i = 0;
		if(i > 3)
		{
			if(i == (rebuf[3] + 5))//�������ݳ����Ƿ����
			{
				if(rebuf[2] == 0x15)//�������ݹ̶�λ�Ƿ���ȷ
				{
					if(!Receive_OK)
					{
						memcpy(DistanceBuff, rebuf, 10);//�������ת������
						Receive_OK = 1;//������ɱ�־λ
					}
				}
				i = 0;
			}
		}
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuf_3, 1);
	}
	
}
uint16_t GetDistance(void)
{
	uint8_t i, sum = 0;
	static uint16_t Distance = 255;
	if(Receive_OK)//���ڽ������
	{
		for(i = 0; i < (DistanceBuff[3] + 4); i++)
		{
			sum += DistanceBuff[i];//У����ж�
		}
		if(sum == DistanceBuff[8])//У����ж�
		{	
			Distance = (DistanceBuff[4]<<8 | DistanceBuff[5]);//ת������ֵ
		}
		Receive_OK = 0;//����������ϱ�־
	}
	return Distance;
}

void DY_SV17F_Init(void)
{
	uint8_t V_MAX[5] = {0xAA, 0x13, 0x01, 0x1E, 0xAC};//�������
	uint8_t V_LOOP[5] = {0xAA, 0x18, 0x01, 0x02, 0xC5};//���β��Ų�ѭ��
	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};//ֹͣ����
	
	HAL_UART_Transmit(&huart2, V_MAX, 5, 0xffff);
	HAL_UART_Transmit(&huart2, V_LOOP, 5, 0xffff);
	HAL_UART_Transmit(&huart2, Stop, 4, 0xffff);
}

void DY_SV17F_Warning_D(void)
{
	uint8_t Sound[] = {0xAA, 0x08, 0x0B, 0x02, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x32, 0x2A, 0x4D, 0x50, 0x33, 0xDA};//���ž�����������
	uint8_t Start[4] = {0xAA, 0x02, 0x00, 0xAC};//��ʼ����
//	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};
	
	HAL_UART_Transmit(&huart2, Sound, sizeof(Sound), 0xffff);
	HAL_UART_Transmit(&huart2, Start, 4, 0xffff);
}

void DY_SV17F_Warning_L(void)
{
	uint8_t Sound[] = {0xAA, 0x08, 0x0F, 0x02, 0x2F, 0x44, 0x59, 0x2A, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x33, 0x2A, 0x4D, 0x50, 0x33, 0xD5};//���Ź�����������
	uint8_t Start[4] = {0xAA, 0x02, 0x00, 0xAC};//��ʼ����
//	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};
	
	HAL_UART_Transmit(&huart2, Sound, sizeof(Sound), 0xffff);
	HAL_UART_Transmit(&huart2, Start, 4, 0xffff);
}
//�����ض��� Ϊ��ʹ��printf����
extern UART_HandleTypeDef huart1;
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
	return ch;
}
int fgetc(FILE *f)
{
	uint8_t ch = 0;
	HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
	return ch;
}
