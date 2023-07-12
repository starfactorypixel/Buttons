#pragma once

#include <UserButtons.h>

#define CLK_H()        	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);	// установить выход SCK(clock) в High
#define CLK_L()         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);	// установить выход SCK(clock) в Low
#define SDO_595_H()        	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);	// установить выход SDO(Data) в High
#define SDO_595_L()     	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);	// установить выход SDO(Data) в Low
#define CE_H()      		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);	// установить выход защелка CE(SH_CP) в High
#define CE_L()       		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);	// установить выход защелка CE(SH_CP) в Low

#define OE_595_H()      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	// установить выход защелка в High
#define OE_595_L()       		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);	// установить выход защелка в Low
#define SRCLR_595_H()      	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);	// установить выход защелка в High
#define SRCLR_595_L()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);	// установить выход защелка в Low

#define SH_LD_165_H()      	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);	// установить выход защелка в High
#define SH_LD_165_L()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);	// установить выход защелка в Low

extern SPI_HandleTypeDef hspi2;

namespace ButtonsLeds
{
	static constexpr uint8_t CFG_Count = 2;

	void hw_spi(uint8_t *buttons, uint8_t *leds, uint8_t length);
	
	UserButtons<CFG_Count> obj(&hw_spi);
	
	void hw_spi(uint8_t *buttons, uint8_t *leds, uint8_t length)
	{
		CE_L();        		// Write enadle
		SH_LD_165_L();		// Reset HC165
		SH_LD_165_H();
		HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) leds, (uint8_t*) buttons, length, 5000);
		CE_H();       		// Write disable

		//Serial::Printf("led: %02X %02X\r\n", leds[0], leds[1]);
	}
	
/*
	Port map
		[09]	[13]	[01]	[05]
		[10]	[14]	[02]	[06]
		[11]	[15]	[03]	[07]
		[12]	[16]	[04]	[08]

					[ SWD ]
*/

	void OnChange(uint8_t port, bool state)
	{
		Logger.PrintTopic("BTN").Printf("port: %2d, state: %d;", port, state).PrintNewLine();
		
		obj.SetLedState(port, state);

		uint8_t set_byte = (state == true) ? 0xFF : 0x00;
		switch (port)
		{
			case 1:
			{
				// Ближний свет
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C5, set_byte);
				break;
			}
			case 2:
			{
				// Габариты, перёд, зад.
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C4, set_byte);
				CANLib::can_manager_light.SendSet<uint8_t>(0x00E4, set_byte);
				break;
			}
			case 3:
			{
				// Свет в салоне
				break;
			}
			case 4:
			{
				// Клаксон
				break;
			}
			case 5:
			{
				// Аварийка
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C9, set_byte);
				CANLib::can_manager_light.SendSet<uint8_t>(0x00E9, set_byte);
				break;
			}
			case 6:
			{
				// Вентилятор
				break;
			}
			case 7:
			{
				// Левый поворотник
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C7, set_byte);
				CANLib::can_manager_light.SendSet<uint8_t>(0x00E7, set_byte);
				break;
			}
			case 8:
			{
				// Педаль тормоза
				CANLib::can_manager_light.SendSet<uint8_t>(0x00E5, set_byte);
				break;
			}
			case 9:
			{
				// Кнопка открытия капота
				break;
			}
			case 10:
			{
				// Кнопка открытия багажника
				break;
			}
			case 11:
			{
				// Кнопка открытия левой двери
				break;
			}
			case 12:
			{
				// Правый поворотник
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C8, set_byte);
				CANLib::can_manager_light.SendSet<uint8_t>(0x00E8, set_byte);
				break;
			}
			case 13:
			{
				// Концевик левая дверь
				break;
			}
			case 14:
			{
				// Концевик правая дверь
				break;
			}
			case 15:
			{
				// Кнопка открытия правой двери
				break;
			}
			case 16:
			{
				// Дальний свет
				CANLib::can_manager_light.SendSet<uint8_t>(0x00C6, set_byte);
				break;
			}
		}
		
		return;
	}
	
	inline void Setup()
	{
		// Reset shift register 74HC595 & 74HC165
		SRCLR_595_L();
		OE_595_L();
		SRCLR_595_H();
		SH_LD_165_H();
		
		obj.RegChangeFunction(&OnChange);
		//obj.SetButtonMode(2, obj.MODE_TRIGGER);
		
		return;
	}
	
	inline void Loop(uint32_t &current_time)
	{
		obj.Processing(current_time);
		
		current_time = HAL_GetTick();
		
		return;
	}
}
