/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include <String.h>
#include <stdio.h>
#include "stm32f3xx_hal.h" // change this here and inside LiquidCrystal library accordingly
#include "LiquidCrystal.h"
typedef struct {
	uint16_t frequency;
	uint16_t duration;
} Tone;
typedef unsigned char byte;
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart1;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
TIM_HandleTypeDef *pwm_timer = &htim2;
uint32_t pwm_channel = TIM_CHANNEL_1;   // Select configured PWM channel number
const Tone *volatile melody_ptr;
volatile uint16_t melody_tone_count;
volatile uint16_t current_tone_number;
volatile uint32_t current_tone_end;
volatile uint32_t last_button_press;
char lcdstr[20];
char str[20];   //for UART
char name[10];   //for name
int new_field[20][4]; //0 = empty 1 = spaceship 2=enemy 3 = spaceship bullet 4=enemy bullet
int old_field[20][4]; //0 = empty 1 = spaceship 2=enemy 3 = spaceship bullet 4=enemy bullet
int load = 0;
int last_gpio_exti = 0;
int i = 0, j = 0, t = 0, sw = 0, length;
int keypad = 0;
int heart = 7;
int movetime = 2;
int volume = 50;          // (0 - 1000)
int killed = 0;
int music = 0;
enum levels {
	Easy, Medium, Hard
};
enum pages {
	Start, About, End, Level, Name, Game, Menu
};
enum pages page;
enum levels level;
char strlevel[3][6] = { "EASY  ", "Medium", "Hard  " };
RTC_TimeTypeDef mytime;
RTC_DateTypeDef mydate;
byte spaceship[8] = { 0x00, 0x00, 0x07, 0x1F, 0x1F, 0x07, 0x00, 0x00 };
byte bullet[8] = { 0x00, 0x00, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00 };
byte enemy[8] = { 0x00, 0x10, 0x09, 0x0E, 0x0E, 0x09, 0x10, 0x00 };
byte e_bullet[8] = { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 };
byte crash[8] = { 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00, 0x00 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
/* USER CODE BEGIN PFP */
const Tone melody[] = {         // Dart Vader theme (Imperial March) - Star wars
		{ NOTE_AS4, 125 }, { NOTE_AS4, 125 }, { NOTE_AS4, 125 },  //1
				{ NOTE_F5, 500 }, { NOTE_C6, 500 }, { NOTE_AS5, 125 }, {
				NOTE_A5, 125 }, { NOTE_G5, 125 }, { NOTE_F6, 500 }, {
				NOTE_C6, 250 }, { NOTE_AS5, 125 }, { NOTE_A5, 125 }, {
				NOTE_G5, 125 }, { NOTE_F6, 500 }, { NOTE_C6, 250 }, {
				NOTE_AS5, 125 }, { NOTE_A5, 125 }, { NOTE_AS5, 125 }, {
				NOTE_G5, 125 }, { NOTE_C5, 125 }, { NOTE_C5, 125 }, {
				NOTE_C5, 125 }, { NOTE_F5, 500 }, { NOTE_C6, 500 }, {
				NOTE_AS5, 125 }, { NOTE_A5, 125 }, { NOTE_G5, 125 }, {
				NOTE_F6, 500 }, { NOTE_C6, 250 }, { NOTE_AS5, 125 }, {
				NOTE_A5, 125 }, { NOTE_G5, 125 }, { NOTE_F6, 500 }, {
				NOTE_C6, 250 }, //8
				{ NOTE_AS5, 125 }, { NOTE_A5, 125 }, { NOTE_AS5, 125 }, {
				NOTE_G5, 500 }, { NOTE_C5, -125 }, { NOTE_C5, 62 }, {
				NOTE_D5, -250 }, { NOTE_D5, 125 }, { NOTE_AS5, 125 }, {
				NOTE_A5, 125 }, { NOTE_G5, 125 }, { NOTE_F5, 125 }, {
				NOTE_F5, 125 }, { NOTE_G5, 125 }, { NOTE_A5, 125 }, {
				NOTE_G5, 250 }, { NOTE_D5, 125 }, { NOTE_E5, 250 }, {
				NOTE_C5, -125 }, { NOTE_C5, 62 }, { NOTE_D5, -250 }, {
				NOTE_D5, 125 }, { NOTE_AS5, 125 }, { NOTE_A5, 125 }, {
				NOTE_G5, 125 }, { NOTE_F5, 125 }, { NOTE_C6, -125 }, {
				NOTE_G5, 62 }, { NOTE_G5, 500 }, { REST, 125 }, {
				NOTE_C5, 125 }, //13
				{ NOTE_D5, -250 }, { NOTE_D5, 125 }, { NOTE_AS5, 125 }, {
				NOTE_A5, 125 }, { NOTE_G5, 125 }, { NOTE_F5, 125 }, {
				NOTE_F5, 125 }, { NOTE_G5, 125 }, { NOTE_A5, 125 }, {
				NOTE_G5, 250 }, { NOTE_D5, 125 }, { NOTE_E5, 250 }, {
				NOTE_C6, -125 }, { NOTE_C6, 62 }, { NOTE_F6, 250 }, {
				NOTE_DS6, 125 }, { NOTE_CS6, 250 }, { NOTE_C6, 125 }, {
				NOTE_AS5, 250 }, { NOTE_GS5, 125 }, { NOTE_G5, 250 }, {
				NOTE_F5, 125 }, { NOTE_C6, 1000 } };
//int notes = sizeof(melody) / sizeof(melody[0]) / 2;
void spaceship_shooter() {
	int x;
	for (x = 0; x < 4; x++) {
		for (i = 1; i <= 18; i++) {
			if (new_field[i][x] == 3) {
				if (new_field[i - 1][x] == 2) {  //kill enemy
					new_field[i - 1][x] = 0;
					new_field[i][x] = 0;
					killed++;
				} else if (new_field[i - 1][x] == 4) {  //crash bullets
					new_field[i - 1][x] = 5;
					new_field[i][x] = 0;
				} else if (new_field[i - 1][x] == 0
						|| new_field[i - 1][x] == 5) {  //move bullet
					new_field[i - 1][x] = 3;
					new_field[i][x] = 0;
				}
			}
		}
	}
}
void move_enemy() {
	int x;
	for (x = 0; x < 4; x++) {
		for (i = 18; i >= 0; i--) {
			if (new_field[i][x] == 2 && new_field[i + 1][x] != 1) { // move down
				new_field[i + 1][x] = 2;
				new_field[i][x] = 0;
			}
		}
	}
	for (x = 0; x < 4; x++) {
		new_field[0][x] = 2;
	}
}
void enemy_shooter() {
	int x , y = 0;
	for (x = 0; x < 4; x++) {
		for (i = 0; i <= 18; i++) {
			if (new_field[i][x] == 4) {
				if (new_field[i + 1][x] == 0 && y < 4) {  //move down
					new_field[i + 1][x] = 4;
					new_field[i][x] = 0;
					y ++;
				} else if (new_field[i + 1][x] == 3&& y < 4) {  //crash bullets
					new_field[i + 1][x] = 5;
					new_field[i][x] = 0;
				}
				else if (new_field[i + 1][x] == 1) {  //end game
					new_field[i + 1][x] = 1;
					new_field[i][x] = 0;
					heart --;
					if(heart == 0)
						new_field[i + 1][x] = 5;
				}
			}
		}
	}
}
void shoot_enemy() {
	int x;
	for (x = 0; x < 4; x++) {
		for (i = 3; i < 18; i++) {
			if (new_field[i][x] == 2 && new_field[i + 1][x] != 2) {
				new_field[i + 1][x] = 4;
			}
		}
	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC2) {
		volume = HAL_ADC_GetValue(&hadc2) / 5;
		//length = sprintf(str, "vol = %d", vol);
		//HAL_UART_Transmit(&huart1, str, length, 1000);
		HAL_ADC_Start_IT(&hadc2);
	}
}
void send_field() {
	length = sprintf(str, "page = %d", page);
	HAL_UART_Transmit(&huart1, str, length, 1000);
	/*for (int i = 0; i < 20; i++) {
		length = sprintf(str, "\n{%d,%d,%d,%d}", new_field[i][0],
				new_field[i][1], new_field[i][2], new_field[i][3]);
		HAL_UART_Transmit(&huart1, str, length, 1000);
	}*/
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//show display f = 20Hz
	if (htim->Instance == TIM1) {
		//print
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		send_field();
		//end game
		if (killed >= (15 + (int) level * 10) && page == Game) {
			page = End;
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 1);
		}
		if (heart <= 0) {
			page = End;
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);

		}
		for (j = 0; j < 4; j++) {
			if (new_field[19][j] == 2) {
				page = End;
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
			}
		}
		if (page == Name) {
			setCursor(0, 0);
			sprintf(lcdstr, "Enter Your Name:    ");
			print(lcdstr);
			setCursor(0, 1);
			print("                    ");
			setCursor(0, 2);
			print("                    ");
		} else if (page == Level) {
			setCursor(0, 0);
			sprintf(lcdstr, "Level:              ");
			print(lcdstr);
			setCursor(0, 1);
			if (level == Easy)
				print("EASY                ");
			else if (level == Medium)
				print("MEDIUM              ");
			else if (level == Hard)
				print("HARD                ");
		} else if (page == About) {
			HAL_RTC_GetTime(&hrtc, &mytime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &mydate, RTC_FORMAT_BIN);
			setCursor(0, 0);
			sprintf(lcdstr, "%2d:%2d:%2d", mytime.Hours, mytime.Minutes,
					mytime.Seconds);
			print(lcdstr);
			setCursor(0, 1);
			sprintf(lcdstr, "%d/%d/%d       ", mydate.Year, mydate.Month,
					mydate.Date);
			print(lcdstr);
			setCursor(0, 2);
			print("M.Akhavan & S.Anvari    ");
		} else if (page == Menu) {
			//music++;
			setCursor(0, 0);
			print("Choose              1.About Us2.New Game");
			//if (music % 8 == 0)
			///	Change_Melody(melody, ARRAY_LENGTH(melody));

		} else if (page == End) {
			setCursor(0, 0);
			if (killed >= (15 + (int) level * 10)) {
				print("You Won!            "); //
				setCursor(0, 1);
				print(name);
				print("          ");
				setCursor(0, 2);
				sprintf(lcdstr, "killed: %d           ", killed);
				print(lcdstr);
				setCursor(0, 3);
				sprintf(lcdstr, "Level: %c%c%c%c%c%c       ",
						strlevel[level][0], strlevel[level][1],
						strlevel[level][2], strlevel[level][3],
						strlevel[level][4], strlevel[level][5]);
				print(lcdstr);
				length = sprintf(str, "\nYou Wooooooooon :)");
				HAL_UART_Transmit(&huart1, str, length, 1000);
			} else {
				print("Game Over!          ");
				setCursor(0, 1);
				print(name);
				print("          ");
				setCursor(0, 2);
				sprintf(lcdstr, "killed: %d           ", killed);
				print(lcdstr);
				setCursor(0, 3);
				sprintf(lcdstr, "Level: %c%c%c%c%c%c       ",
						strlevel[level][0], strlevel[level][1],
						strlevel[level][2], strlevel[level][3],
						strlevel[level][4], strlevel[level][5]);
				print(lcdstr);
				length = sprintf(str, "\nSorry ,Try Better :(");
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}

		}else if (page == Game) {
			sw++;
			if (sw >= 100000)
				sw = 0;
			if (sw % (movetime) == 0) {

				move_enemy();
			}
			//enemy_shooter();

			//shoot_enemy();

			spaceship_shooter();

			for (i = 0; i < 20; i++) {
				for (j = 0; j < 4; j++) {
					if (new_field[i][j] != old_field[i][j]) {
						old_field[i][j] = new_field[i][j];
						setCursor(i, j);
						if (new_field[i][j] == 0) {
							print(" ");
						}
						if (new_field[i][j] == 1) {
							write(0);

						}
						if (new_field[i][j] == 2) {
							write(1);

						}
						if (new_field[i][j] == 3) {
							write(2);
						}
						if (new_field[i][j] == 4) {
							write(3);
						}
						if (new_field[i][j] == 5) {
							write(4);
						}
					}
				}
			}
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (last_gpio_exti + 200 > HAL_GetTick()) // Simple button debouncing{
		return;
	last_gpio_exti = HAL_GetTick();
	if (GPIO_Pin == GPIO_PIN_0) { //left
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_14);

		if (page == Start) {
			page = Menu; //go to menu
			Change_Melody(melody, ARRAY_LENGTH(melody));

		} else if (page == Menu || page == About) {
			page = Name; // go to game
			PWM_Change_Tone(0,0);
		} else if (page == Game) {
			for (int x = 0; x < 4; x++) {
				if (new_field[19][x] == 1) {
					new_field[19][x] = 0;
					new_field[19][(x + 3) % 4] = 1;
				}
			}
			if ((15 + (int) level * 10) - killed == 3) {
				length = sprintf(str, "\n Just kill %d enemy more",
						((15 + (int) level * 10) - killed));
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
			if (heart <= 1) {
				length = sprintf(str, "\n vaaaaaaaaaaaaaaay heart = %d", heart);
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
		} else if (page == Name) {
			HAL_UART_Receive(&huart1, name, 10, HAL_MAX_DELAY);

			page = Level;
		} else if (page == Level) {
			page = Game;
			length = sprintf(str,
					"\nWelcome! \nTarget = %d  Level = %c%c%c%c%c%c",
					(15 + (int) level * 10), strlevel[level][0],
					strlevel[level][1], strlevel[level][2], strlevel[level][3],
					strlevel[level][4], strlevel[level][5]);
			HAL_UART_Transmit(&huart1, str, length, 1000);
		}
	}
	if (GPIO_Pin == GPIO_PIN_1) { //right
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);

		if (page == Start) {
			page = Menu;
			Change_Melody(melody, ARRAY_LENGTH(melody));

		} else if (page == About) {
			page = Name; // go to game

		} else if (page == Name) {
			HAL_UART_Receive_IT(&huart1, name, 10);
			//length = sprintf(str, "\n name = %s", name);
			//	HAL_UART_Transmit(&huart1, str, length, 1000);
			page = Level; // go to game

		} else if (page == Level) {
			length = sprintf(str, "\n level = %d", level);
			//HAL_UART_Transmit(&huart1, str, length, 1000);
			if (level == Easy) {
				level = Medium;
				heart = 5;
				movetime = 3;
			} else if (level == Medium) {
				level = Hard;
				heart = 3;
				movetime = 1;
			} else if (level == Hard) {
				level = Easy;
				heart = 7;
				movetime = 5;
			}

		} else if (page == Menu) {
			page = About; //go to about
			PWM_Change_Tone(0,0);
		} else if (page == Game) {
			for (int x = 0; x < 4; x++) {
				if (new_field[19][x] == 1) {
					new_field[19][x] = 0;
					new_field[19][(x + 1) % 4] = 1;
				}
			}
			if ((15 + (int) level * 10) - killed == 3) {
				length = sprintf(str, "\n Just kill %d enemy more",
						((15 + (int) level * 10) - killed));
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
			if (heart <= 1) {
				length = sprintf(str, "\n vaaaaaaaaaaaaaaay heart = %d", heart);
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
		}
	}
	if (GPIO_Pin == GPIO_PIN_2) { //shoot
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);

		if (page == Game) {
			for (int x = 0; x < 4; x++) {
				if (new_field[19][x] == 1) {
					new_field[18][x] = 3;

				}
			}
			if ((15 + (int) level * 10) - killed == 3) {
				length = sprintf(str, "\n Just kill %d enemy more",
						((15 + (int) level * 10) - killed));
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
			if (heart <= 1) {
				length = sprintf(str, "\n vaaaaaaaaaaaaaaay heart = %d", heart);
				HAL_UART_Transmit(&huart1, str, length, 1000);
			}
		}
	}
}
void PWM_Start() {
	HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}
void PWM_Change_Tone(uint16_t pwm_freq, uint16_t volume) // pwm_freq (1 - 20000), volume (0 - 1000)
{
	if (pwm_freq == 0 || pwm_freq > 20000) {
		__HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, 0);
	} else {
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
		const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();
		// const uint16_t prescaler = 1;
		const uint16_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
		const uint32_t timer_clock = internal_clock_freq / prescaler;
		const uint32_t period_cycles = timer_clock / pwm_freq;
		const uint32_t pulse_width = volume * period_cycles / 1000 / 2;

		pwm_timer->Instance->PSC = prescaler - 1;
		pwm_timer->Instance->ARR = period_cycles - 1;
		pwm_timer->Instance->EGR = TIM_EGR_UG;
		__HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, pulse_width); // pwm_timer->Instance->CCR2 = pulse_width;
	}
}
void Change_Melody(const Tone *melody, uint16_t tone_count) {
	melody_ptr = melody;
	melody_tone_count = tone_count;
	current_tone_number = 0;
}

void Update_Melody() {
	if ((HAL_GetTick() > current_tone_end)
			&& (current_tone_number < melody_tone_count)) {
		const Tone active_tone = *(melody_ptr + current_tone_number);
		PWM_Change_Tone(active_tone.frequency, volume);
		current_tone_end = HAL_GetTick() + active_tone.duration;
		current_tone_number++;
	}
}
/*
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
 if (huart->instance == USART2)
 {
 HAL_UART_Receive_IT(&huart2, str, sizeof(str));
 }
 }

 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
 if (GPIO_Pin == GPIO_PIN_0)
 {

 }
 } */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_USB_PCD_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_RTC_Init();
	MX_ADC2_Init();
	MX_TIM3_Init();
	MX_USART1_UART_Init();
	MX_TIM4_Init();
	MX_TIM8_Init();
	/* USER CODE BEGIN 2 */
	//initialize
	LiquidCrystal(GPIOD, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
	GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14);
	begin(20, 4);
	page = Start;
	level = Easy;
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim8);
	HAL_ADC_Start_IT(&hadc2);
	PWM_Start();
	mytime.Hours = 7;
	mytime.Minutes = 20;
	mytime.Seconds = 40;
	mydate.Year = 22;
	mydate.Month = 12;
	mydate.Date = 28;
	HAL_RTC_SetTime(&hrtc, &mytime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &mydate, RTC_FORMAT_BIN);
	//build new_field
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 4; j++) {
			old_field[i][j] = -1;
		}
	}
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 4; j++) {
			new_field[i][j] = 0;
			if (i < 4) {
				new_field[i][j] = 2;
			}
		}
	}
	new_field[19][2] = 1;

	createChar(0, spaceship);
	createChar(1, enemy);
	createChar(2, bullet);
	createChar(3, e_bullet);
	createChar(4, crash);

	setCursor(0, 0);
	print("                    ");
	setCursor(0,1);
	print("                    ");
	setCursor(0,2);
	print("   SPACE INVADERS   ");
	setCursor(0,3);
	print("                    ");

	//FILE  *f = fopen("document.txt", "w");
	//fprintf(f, "\n...SPACE INVADERS...\n Creators: M.Akhavafard & Sh.Anvari \nLevel: %d\nkillded: %d\nheart:%d", level,killed,heart);
	//fclose(f);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB
			| RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_RTC
			| RCC_PERIPHCLK_TIM1 | RCC_PERIPHCLK_TIM8 | RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
	PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC2_Init(void) {

	/* USER CODE BEGIN ADC2_Init 0 */

	/* USER CODE END ADC2_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC2_Init 1 */

	/* USER CODE END ADC2_Init 1 */
	/** Common config
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.NbrOfConversion = 1;
	hadc2.Init.DMAContinuousRequests = DISABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	if (HAL_ADC_Init(&hadc2) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC2_Init 2 */

	/* USER CODE END ADC2_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x2000090E;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK) {
		Error_Handler();
	}
	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void) {

	/* USER CODE BEGIN RTC_Init 0 */

	/* USER CODE END RTC_Init 0 */

	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };

	/* USER CODE BEGIN RTC_Init 1 */

	/* USER CODE END RTC_Init 1 */
	/** Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN Check_RTC_BKUP */

	/* USER CODE END Check_RTC_BKUP */

	/** Initialize RTC and set the Time and Date
	 */
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 0x0;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 2 */

	/* USER CODE END RTC_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 7199;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 9999;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 959;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 9999;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 4799;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 9999;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void) {

	/* USER CODE BEGIN TIM8_Init 0 */

	/* USER CODE END TIM8_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM8_Init 1 */

	/* USER CODE END TIM8_Init 1 */
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 0;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 65535;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim8) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM8_Init 2 */

	/* USER CODE END TIM8_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 38400;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief USB Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_PCD_Init(void) {

	/* USER CODE BEGIN USB_Init 0 */

	/* USER CODE END USB_Init 0 */

	/* USER CODE BEGIN USB_Init 1 */

	/* USER CODE END USB_Init 1 */
	hpcd_USB_FS.Instance = USB;
	hpcd_USB_FS.Init.dev_endpoints = 8;
	hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
	if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USB_Init 2 */

	/* USER CODE END USB_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE,
			CS_I2C_SPI_Pin | LD4_Pin | LD3_Pin | LD5_Pin | LD7_Pin | LD9_Pin
					| LD10_Pin | LD8_Pin | LD6_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD,
			GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : CS_I2C_SPI_Pin LD4_Pin LD3_Pin LD5_Pin
	 LD7_Pin LD9_Pin LD10_Pin LD8_Pin
	 LD6_Pin */
	GPIO_InitStruct.Pin = CS_I2C_SPI_Pin | LD4_Pin | LD3_Pin | LD5_Pin | LD7_Pin
			| LD9_Pin | LD10_Pin | LD8_Pin | LD6_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : MEMS_INT3_Pin MEMS_INT4_Pin */
	GPIO_InitStruct.Pin = MEMS_INT3_Pin | MEMS_INT4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PC2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PC3 PC6 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PD15 PD0 PD1 PD2 */
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : PD3 PD4 PD5 PD6
	 PD7 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
			| GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_TSC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
