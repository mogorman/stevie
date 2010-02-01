#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

#include "tdtp.h"
#include "uip.h"
#include "timer.h"
#include "pt.h"
#include "psock.h"
#include "witypes.h"
#include "g2100.h"

//PB0 = port 8
//PB1 = port 9 head
//PB2 = dong and port 10?

//#define TDTP_STATE_CLOSE 1

/* controls exported */
/* arg1 = motor						options being 1,2,3 */
/* arg2 = direction					options being 0,1 */
/* arg3 = intensity					options being 0-255 */
/* arg4 = length of time in ms, with 0 meaning forever	options being 0-20,000 */

/* initial packet contains  */
/* Product_id:version_code:mac */

/* state packet matches input packet */

/* command packet gets acked immediately that state was changed */
//dialadong-001|00:1E:C0:00:0B:DB


/* 1,0,255,20000 */

static char * hex_convert(u8_t hex_digit);

static int handle_tdtp_connection(struct tdtp_state *t);
//static void tdtp_blink(void);


//
void tdtp_timer0_isr(void)
{
}

static void tdtp_connect(void)
{
        struct uip_conn *conn;
        uip_ipaddr_t ipaddr;

        uip_ipaddr(&ipaddr, 10, 69, 69, 1);
        conn = uip_connect(&ipaddr, htons(6969));
        if(conn == NULL) {
#ifdef MOG_DEBUG
                printf("failure out of ports\r\n");
#endif
                tdtp_state=1;
        }
}

void tdtp_hw_init(void)
{
	DDRB = _BV(PB1);
}

void tdtp_init(void)
{
        tdtp_connect();
}

void tdtp_appcall(void)
{
        struct tdtp_state *t = &(uip_conn->appstate);
        if(uip_connected()) {
                PSOCK_INIT(&t->p, (uint8_t *)t->inputbuffer, sizeof(t->inputbuffer));
        }
        handle_tdtp_connection(t);
}

static int handle_tdtp_connection(struct tdtp_state *t)
{
        char *p, *tmp, *to_free = NULL;
//        char version[32];
        u8_t new_motor = 0, new_direction = 0, new_pwm = 0, new_time = 0;
        U8 *mac = NULL;
	PSOCK_BEGIN(&t->p);
        if(uip_connected() && !(uip_timedout())
           && !(uip_closed()) && !(uip_aborted())) {
#ifdef MOG_DEBUG
                printf("connectedr\n");
#endif
                mac = zg_get_mac();
                /* sprintf(version, "dialadong-001|%02X:%02X:%02X:%02X:%02X:%02X", */
                /*         mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]); */
                /* uip_send(version, strlen(version)); */
                uip_send("dialadong-001|",15);
                uip_send(hex_convert(mac[0]),3);
                uip_send(hex_convert(mac[1]),3);
                uip_send(hex_convert(mac[2]),3);
                uip_send(hex_convert(mac[3]),3);
                uip_send(hex_convert(mac[4]),3);
                uip_send(hex_convert(mac[5]),3);
                uip_send("dialadong-001|00:1E:C0:00:0B:DB", 32);
        } else if(uip_newdata()) {
                memset(&t->inputbuffer, 0, sizeof(t->inputbuffer));
		PSOCK_READTO(&t->p, '\n');
	#ifdef MOG_DEBUG
		printf("hello %d:%s:\r\n", uip_datalen(), t->inputbuffer);
	#endif
                tmp = to_free = strdup(t->inputbuffer);
                if(tmp) {
                        p = strsep(&tmp, ",");
                        if(p) {
                                new_motor = atoi(p);
                                p = strsep(&tmp, ",");
                        }
                        if(p) {
                                new_direction = atoi(p);
                                p = strsep(&tmp, ",");
                        }
                        if(p) {
                                new_pwm = atoi(p);
                                p = strsep(&tmp, ",");
                        }
                        if(p) {
                                new_time = atoi(p);
#ifdef MOG_DEBUG
                                printf("hello :%d:%d:%d:%d:\r\n", new_motor, new_direction, new_pwm, new_time);
#endif
                                uip_send(t->inputbuffer,sizeof(t->inputbuffer));
                                //do stuff here
                        }
                }
	} else if(uip_closed()) {
#ifdef MOG_DEBUG
                printf("failure closed\r\n");
#endif
		uip_abort();
                tdtp_state = 1;
	} else if(uip_aborted()) {
#ifdef MOG_DEBUG
                printf("failure aborted\r\n");
#endif
		uip_abort();
                tdtp_state = 1;
	} else if(uip_timedout()) {
#ifdef MOG_DEBUG
                printf("failure timedout\r\n");
#endif
		uip_abort();
                tdtp_state = 1;
        }

        if(to_free)
                free(to_free);

	PSOCK_END(&t->p);
	return 0;
}


//
//static void tdtp_blink(void)
//{
//  /* char i; */
//  /* printf("hello you\r\n"); */
//  /* DDRB = _BV(PB0); // make Arduino Pin 13 (Atmega8 PortB bit 5) an output (%0000100) */
//  /*   for(i = 0; i < 10; i++){ */
//  /*     _delay_ms(30);      // max is 262.14 ms / F_CPU in MHz */
//  /*   } */
//  /*   PORTB ^= _BV(PB0);     // put Arduino Pin 13 (Atmega8 PortB bit 5) HIGH (%00100000) */
//  /*   for(i = 0; i < 10; i++){ */
//  /*     _delay_ms(30); */
//  /*   } */
//    PORTB ^= _BV(PB1);     // put Arduino Pin 13 (Atmega8 PortB bit 5) LOW (%00000000)
//}


static char * hex_convert(u8_t hex_digit)
{
        switch(hex_digit) {
        case 0:    return "00";
        case 1:    return "01";
        case 2:    return "02";
        case 3:    return "03";
        case 4:    return "04";
        case 5:    return "05";
        case 6:    return "06";
        case 7:    return "07";
        case 8:    return "08";
        case 9:    return "09";
        case 10:   return "0A";
        case 11:   return "0B";
        case 12:   return "0C";
        case 13:   return "0D";
        case 14:   return "0E";
        case 15:   return "0F";
        case 16:   return "10";
        case 17:   return "11";
        case 18:   return "12";
        case 19:   return "13";
        case 20:   return "14";
        case 21:   return "15";
        case 22:   return "16";
        case 23:   return "17";
        case 24:   return "18";
        case 25:   return "19";
        case 26:   return "1A";
        case 27:   return "1B";
        case 28:   return "1C";
        case 29:   return "1D";
        case 30:   return "1E";
        case 31:   return "1F";
        case 32:   return "20";
        case 33:   return "21";
        case 34:   return "22";
        case 35:   return "23";
        case 36:   return "24";
        case 37:   return "25";
        case 38:   return "26";
        case 39:   return "27";
        case 40:   return "28";
        case 41:   return "29";
        case 42:   return "2A";
        case 43:   return "2B";
        case 44:   return "2C";
        case 45:   return "2D";
        case 46:   return "2E";
        case 47:   return "2F";
        case 48:   return "30";
        case 49:   return "31";
        case 50:   return "32";
        case 51:   return "33";
        case 52:   return "34";
        case 53:   return "35";
        case 54:   return "36";
        case 55:   return "37";
        case 56:   return "38";
        case 57:   return "39";
        case 58:   return "3A";
        case 59:   return "3B";
        case 60:   return "3C";
        case 61:   return "3D";
        case 62:   return "3E";
        case 63:   return "3F";
        case 64:   return "40";
        case 65:   return "41";
        case 66:   return "42";
        case 67:   return "43";
        case 68:   return "44";
        case 69:   return "45";
        case 70:   return "46";
        case 71:   return "47";
        case 72:   return "48";
        case 73:   return "49";
        case 74:   return "4A";
        case 75:   return "4B";
        case 76:   return "4C";
        case 77:   return "4D";
        case 78:   return "4E";
        case 79:   return "4F";
        case 80:   return "50";
        case 81:   return "51";
        case 82:   return "52";
        case 83:   return "53";
        case 84:   return "54";
        case 85:   return "55";
        case 86:   return "56";
        case 87:   return "57";
        case 88:   return "58";
        case 89:   return "59";
        case 90:   return "5A";
        case 91:   return "5B";
        case 92:   return "5C";
        case 93:   return "5D";
        case 94:   return "5E";
        case 95:   return "5F";
        case 96:   return "60";
        case 97:   return "61";
        case 98:   return "62";
        case 99:   return "63";
        case 100:  return "64";
        case 101:  return "65";
        case 102:  return "66";
        case 103:  return "67";
        case 104:  return "68";
        case 105:  return "69";
        case 106:  return "6A";
        case 107:  return "6B";
        case 108:  return "6C";
        case 109:  return "6D";
        case 110:  return "6E";
        case 111:  return "6F";
        case 112:  return "70";
        case 113:  return "71";
        case 114:  return "72";
        case 115:  return "73";
        case 116:  return "74";
        case 117:  return "75";
        case 118:  return "76";
        case 119:  return "77";
        case 120:  return "78";
        case 121:  return "79";
        case 122:  return "7A";
        case 123:  return "7B";
        case 124:  return "7C";
        case 125:  return "7D";
        case 126:  return "7E";
        case 127:  return "7F";
        case 128:  return "80";
        case 129:  return "81";
        case 130:  return "82";
        case 131:  return "83";
        case 132:  return "84";
        case 133:  return "85";
        case 134:  return "86";
        case 135:  return "87";
        case 136:  return "88";
        case 137:  return "89";
        case 138:  return "8A";
        case 139:  return "8B";
        case 140:  return "8C";
        case 141:  return "8D";
        case 142:  return "8E";
        case 143:  return "8F";
        case 144:  return "90";
        case 145:  return "91";
        case 146:  return "92";
        case 147:  return "93";
        case 148:  return "94";
        case 149:  return "95";
        case 150:  return "96";
        case 151:  return "97";
        case 152:  return "98";
        case 153:  return "99";
        case 154:  return "9A";
        case 155:  return "9B";
        case 156:  return "9C";
        case 157:  return "9D";
        case 158:  return "9E";
        case 159:  return "9F";
        case 160:  return "A0";
        case 161:  return "A1";
        case 162:  return "A2";
        case 163:  return "A3";
        case 164:  return "A4";
        case 165:  return "A5";
        case 166:  return "A6";
        case 167:  return "A7";
        case 168:  return "A8";
        case 169:  return "A9";
        case 170:  return "AA";
        case 171:  return "AB";
        case 172:  return "AC";
        case 173:  return "AD";
        case 174:  return "AE";
        case 175:  return "AF";
        case 176:  return "B0";
        case 177:  return "B1";
        case 178:  return "B2";
        case 179:  return "B3";
        case 180:  return "B4";
        case 181:  return "B5";
        case 182:  return "B6";
        case 183:  return "B7";
        case 184:  return "B8";
        case 185:  return "B9";
        case 186:  return "BA";
        case 187:  return "BB";
        case 188:  return "BC";
        case 189:  return "BD";
        case 190:  return "BE";
        case 191:  return "BF";
        case 192:  return "C0";
        case 193:  return "C1";
        case 194:  return "C2";
        case 195:  return "C3";
        case 196:  return "C4";
        case 197:  return "C5";
        case 198:  return "C6";
        case 199:  return "C7";
        case 200:  return "C8";
        case 201:  return "C9";
        case 202:  return "CA";
        case 203:  return "CB";
        case 204:  return "CC";
        case 205:  return "CD";
        case 206:  return "CE";
        case 207:  return "CF";
        case 208:  return "D0";
        case 209:  return "D1";
        case 210:  return "D2";
        case 211:  return "D3";
        case 212:  return "D4";
        case 213:  return "D5";
        case 214:  return "D6";
        case 215:  return "D7";
        case 216:  return "D8";
        case 217:  return "D9";
        case 218:  return "DA";
        case 219:  return "DB";
        case 220:  return "DC";
        case 221:  return "DD";
        case 222:  return "DE";
        case 223:  return "DF";
        case 224:  return "E0";
        case 225:  return "E1";
        case 226:  return "E2";
        case 227:  return "E3";
        case 228:  return "E4";
        case 229:  return "E5";
        case 230:  return "E6";
        case 231:  return "E7";
        case 232:  return "E8";
        case 233:  return "E9";
        case 234:  return "EA";
        case 235:  return "EB";
        case 236:  return "EC";
        case 237:  return "ED";
        case 238:  return "EE";
        case 239:  return "EF";
        case 240:  return "F0";
        case 241:  return "F1";
        case 242:  return "F2";
        case 243:  return "F3";
        case 244:  return "F4";
        case 245:  return "F5";
        case 246:  return "F6";
        case 247:  return "F7";
        case 248:  return "F8";
        case 249:  return "F9";
        case 250:  return "FA";
        case 251:  return "FB";
        case 252:  return "FC";
        case 253:  return "FD";
        case 254:  return "FE";
        case 255:  return "FF";
        }
        return "";
}
