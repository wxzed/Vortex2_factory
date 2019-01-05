
#ifndef HT1632C_MODE_H__
#define HT1632C_MODE_H__

#include "neopixel_mode.h"

#define Display_DATA_Pin  22
#define Display_WR_Pin    20
#define Display_RD_Pin    17
#define Display_CS_Pin    15


#define COM_SIZE 8
#define OUT_SIZE 16
#define NUM_CHANNEL 1
#define USE_NMOS 1

#define PIXELS_PER_BYTE 8
#define ADDR_SPACE_SIZE (COM_SIZE * OUT_SIZE / PIXELS_PER_BYTE)

#define HT1632_ID_CMD 0b100	/* ID = 100 - Commands */
#define HT1632_ID_RD  0b110	/* ID = 110 - Read RAM */
#define HT1632_ID_WR  0b101	/* ID = 101 - Write RAM */
#define HT1632_ID_LEN 3         /* IDs are 3 bits */

#define HT1632_CMD_SYSDIS 0x00	/* CMD= 0000-0000-x Turn off oscil */
#define HT1632_CMD_SYSEN  0x01	/* CMD= 0000-0001-x Enable system oscil */
#define HT1632_CMD_LEDOFF 0x02	/* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03	/* CMD= 0000-0011-x LEDs ON */
#define HT1632_CMD_BLOFF  0x08	/* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09	/* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10	/* CMD= 0001-00xx-x Slave Mode */
#define HT1632_CMD_MSTMD  0x14 /* CMD= 0001-01xx-x Master Mode, on-chip clock */
#define HT1632_CMD_RCCLK  0x18  /* CMD= 0001-10xx-x Master Mode, external clock */
#define HT1632_CMD_EXTCLK 0x1C	/* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS10 0x28	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM_T  0xA0	/* CMD= 101x-PPPP-x PWM duty cycle - template*/
#define HT1632_CMD_PWM(lvl) (HT1632_CMD_PWM_T | (lvl-1))
  /* Produces the correct command from the given value of lvl. lvl = [0..15] */
#define HT1632_CMD_LEN    8	/* Commands are 8 bits long, excluding the trailing bit */
#define HT1632_ADDR_LEN   7	/* Addresses are 7 bits long */
#define HT1632_WORD_LEN   4     /* Words are 4 bits long */



extern uint8_t const ht1632c_TurnLeft_data[];
extern uint8_t const ht1632c_TurnRight_data[];
extern uint8_t const ht1632c_Advance_data[];

void ht1632_Init(void);
void ht1632c_begin(uint8_t pinCS1, uint8_t pinWR, uint8_t pinDATA); 
void ht1632_show_image(uint8_t const * data);

#endif 
