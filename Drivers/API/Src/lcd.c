/*
 * API_lcd.c
 *
 *  Created on: 10 abr 2026
 *      Author: vicks
 */

#include "lcd.h"

#define MODO_4BIT 0x28
#define RETURN_HOME 0x02
#define ENTRY_MODE 0x04
#define AUTOINCREMENT 0x02
#define CONTROL_DISPLAY 0x08
#define DISPLAY_ON 0x04
#define CLEAR_LCD 0x01
#define COLUMNS 16
#define ROWS 2
#define CLR_DELAY 5
#define LINEA2 0x40
#define LINEA1 0x00
#define SET_CURSORCMD 0x80
#define FILA1 0
#define FILA2 1
#define COL_INICIO 0


static uint8_t LCD_INIT_CMD[]= {MODO_4BIT, CONTROL_DISPLAY, RETURN_HOME, ENTRY_MODE|AUTOINCREMENT,
		CONTROL_DISPLAY|DISPLAY_ON,CLEAR_LCD};


static void LCD_send_nibble(uint8_t nibble, uint8_t rs)
{

	uint8_t data=0;
	data = (nibble<<4);
	if(rs){
		data|=(1<<0);
	}
	data|=(1<<3); //Enciende el backlight
	data|=(1<<2); // para que el LCD lea los datos necesita pasar el EN de 1 a 0
	I2CWriteByte(data);
	LCD_delay(1);

	data &=~(1<<2);
	I2CWriteByte(data);
	LCD_delay(1);
}

static void LCD_send_byte(uint8_t byte, uint8_t rs){
	LCD_send_nibble(byte >> 4, rs); //envía el nibble alto
	LCD_send_nibble(byte & 0x0F, rs);  // envia el nibble bajo
}


static void LCD_write_command(uint8_t cmd)
{
    LCD_send_byte(cmd, 0); // RS = 0
    LCD_delay(2);
}

static void LCD_write_data(uint8_t data)
{
    LCD_send_byte(data, 1); // RS = 1
    LCD_delay(2);
}

void LCD_delay(uint8_t timeD){
	I2CDelay(timeD);
}

/*Funcion para posicionar el cursor
 * el comando para mover el cursor es 0x80
 * linea 1 0x00 a 0x0F entonces comando 0x80+col
 * linea 2 0x40 a 0x4F entonces el comando es 0xC0+col
 * 0x80+0x40=0xC0

 * */
void LCD_setCursor(uint8_t row, uint8_t col){
	uint8_t addr;
	if(row==0){
		addr=LINEA1 + SET_CURSORCMD+col;
	}else{
		addr= LINEA2 + SET_CURSORCMD+ col;
	}
	LCD_write_command(addr);
}


//Sobre escribe la linea completa a partir de situarse en una fila y columna
void LCD_writeStringRC(uint8_t row, uint8_t col, const char *str){

	if (str == NULL || row >= ROWS || col >= COLUMNS) {
		return;
	}

	LCD_setCursor(row,col);
	uint8_t count=col;
	while(*str && count < COLUMNS){
		LCD_write_data((uint8_t)*str++);
		count++;
	}
	//Rellenar con esacios hasta el final de la linea
	while(count<COLUMNS){
		LCD_write_data(' ');
		count++;
	}
}

//Funcion que escibre el string recibido en la fila 1 - wrapper de LCD_writeStringRC
void LCD_writeStringF1(const char *str){
	LCD_writeStringRC(FILA1, COL_INICIO, str);
}


//Funcion que escibre el string recibido en la fila 2 - wrapper de LCD_writeStringRC
void LCD_writeStringF2(const char *str){
	LCD_writeStringRC(FILA2, COL_INICIO,str);
}


//


void LCD_clear(){
	LCD_write_command(CLEAR_LCD);
	LCD_delay(CLR_DELAY);

}

void LCD_writeString(char * str){
	if (str == NULL)
	{
		return;
	}
	LCD_setCursor(0,0);
	 while(*str)   // mientras no sea '\0'
	    {
	        LCD_write_data((uint8_t)*str);
	        str++;
	    }

}

void LCD_init(){
	LCD_delay(20);
	LCD_send_nibble(0x03, 0);
	LCD_delay(10);
    LCD_send_nibble(0x03, 0);
    LCD_delay(1);
    LCD_send_nibble(0x03, 0);
    LCD_delay(1);
    LCD_send_nibble(0x02, 0);
    uint8_t size = sizeof(LCD_INIT_CMD)/sizeof(LCD_INIT_CMD[0]);
    for( uint8_t i=0; i<size; i++)
    {
    	LCD_write_command(LCD_INIT_CMD[i]);
    }


}
