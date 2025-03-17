
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
		rebuf[i++] = aRxBuf_3;//储存接收数据
		if(rebuf[0] != 0x5a)//判断是否为数据起始
			i = 0;
		if((i == 2) && (rebuf[1] != 0x5a))//判断数据格式正确
			i = 0;
		if(i > 3)
		{
			if(i == (rebuf[3] + 5))//检验数据长度是否符合
			{
				if(rebuf[2] == 0x15)//检验数据固定位是否正确
				{
					if(!Receive_OK)
					{
						memcpy(DistanceBuff, rebuf, 10);//接收完成转移数据
						Receive_OK = 1;//接收完成标志位
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
	if(Receive_OK)//串口接收完毕
	{
		for(i = 0; i < (DistanceBuff[3] + 4); i++)
		{
			sum += DistanceBuff[i];//校验和判断
		}
		if(sum == DistanceBuff[8])//校验和判断
		{	
			Distance = (DistanceBuff[4]<<8 | DistanceBuff[5]);//转换距离值
		}
		Receive_OK = 0;//处理数据完毕标志
	}
	return Distance;
}

void DY_SV17F_Init(void)
{
	uint8_t V_MAX[5] = {0xAA, 0x13, 0x01, 0x1E, 0xAC};//音量最大
	uint8_t V_LOOP[5] = {0xAA, 0x18, 0x01, 0x02, 0xC5};//单次播放不循环
	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};//停止播放
	
	HAL_UART_Transmit(&huart2, V_MAX, 5, 0xffff);
	HAL_UART_Transmit(&huart2, V_LOOP, 5, 0xffff);
	HAL_UART_Transmit(&huart2, Stop, 4, 0xffff);
}

void DY_SV17F_Warning_D(void)
{
	uint8_t Sound[] = {0xAA, 0x08, 0x0B, 0x02, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x32, 0x2A, 0x4D, 0x50, 0x33, 0xDA};//播放距离提醒语音
	uint8_t Start[4] = {0xAA, 0x02, 0x00, 0xAC};//开始播放
//	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};
	
	HAL_UART_Transmit(&huart2, Sound, sizeof(Sound), 0xffff);
	HAL_UART_Transmit(&huart2, Start, 4, 0xffff);
}

void DY_SV17F_Warning_L(void)
{
	uint8_t Sound[] = {0xAA, 0x08, 0x0F, 0x02, 0x2F, 0x44, 0x59, 0x2A, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x33, 0x2A, 0x4D, 0x50, 0x33, 0xD5};//播放光照提醒语音
	uint8_t Start[4] = {0xAA, 0x02, 0x00, 0xAC};//开始播放
//	uint8_t Stop[4] = {0xAA, 0x04, 0x00, 0xAE};
	
	HAL_UART_Transmit(&huart2, Sound, sizeof(Sound), 0xffff);
	HAL_UART_Transmit(&huart2, Start, 4, 0xffff);
}
//串口重定向 为了使用printf函数
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
