#include "lcd.h"
#include <Arduino.h>
#include <Wire.h>
#include "font.c"
#include "picture.c"

/******************************************
*******************************************/
lcd::lcd(){
  
}

/******************************************
 * Write a command to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd::WriteByte_command(int dat){
  Wire.beginTransmission(addr);      // transmit to device 0x3f
  Wire.write(0x00);                  // Co=0,A0=0. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(dat);                   // sends restart command. 
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * Init the LCD.
 * This initialization function is called when using LCD.
*******************************************/
void lcd::Init(){
  Wire.begin();
  delay(10);
  WriteByte_command(restart);     //resets Start Line (S[5:0]), Column Address (X[7:0]), 
  delay(10);                      //Page Address (Y[3:0]) and COM Direction (MY) to their default setting.
  
  WriteByte_command(BS);          //select bias
  WriteByte_command(seg);         //seg output scan direction setting
  WriteByte_command(com);         //com output scan direction setting
  
  /********Adjust display brightness********/
  WriteByte_command(regRatio);    //Controls the regulation ratio of the built-in regulator.
  WriteByte_command(EV1);
  WriteByte_command(EV2);

  WriteByte_command(powerCon1);   //Controls the built-in power circuits.
  WriteByte_command(powerCon2);
  WriteByte_command(powerCon3);
  Clear();                        //initalize DDRAM
  
  /**********display setting mode**********/
  WriteByte_command(enter_EC);    //enter extension command set.
  WriteByte_command(DSM_ON);      //display settin mode on.
  WriteByte_command(exit_EC);     //exit extension command set.
  WriteByte_command(DT);          //set the display duty, DT=33.
  WriteByte_command(BA);          //This command has priority over the Bias Select (BS).
  WriteByte_command(FR);          //specifies the frame rate for duty. duty=250.
  //*/

  WriteByte_command(displayON);   //display on.
  WriteByte_command(startLine);   //Set display start line.
}

/******************************************
 * Used to test screen pixels.
*******************************************/
void lcd::testPixel(int t){
  for(int x=0; x<4; x++){
    WriteByte_command(0xb0 + x);  //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows

    /*******automatically increased by one******/
    WriteByte_command(0x10);      //x, column address x=0-0-0-0-1-x7-x6-x5-x4
    WriteByte_command(0x00);      //x, column address x=0-0-0-0-0-x3-x2-x1-x0
    
    for(int i=0; i<128; i++){
      WriteByte_dat(0xff);        //row=bit0--bit7, display on
      delay(t);
      }  
  }
  for(int x=0; x<4; x++){
    WriteByte_command(0xb0 + x);
    WriteByte_command(0x00);
    WriteByte_command(0x10);
    for(int i=0; i<128; i++){
      WriteByte_dat(0x00);       //row=bit0--bit7, display off
      delay(t);
      }  
  }
}

/******************************************
 * Write a data to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd::WriteByte_dat(int dat){
  Wire.beginTransmission(addr);      // transmit to device 0x7e
  Wire.write(0x40);                  // Co=0,A0=1. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(dat);                   // sends data. 
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * Continuously write data to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd::WriteCont_dat(int str[]){
  int len = 0;
  len = sizeof(str);
  Wire.beginTransmission(addr);      // transmit to device 0x7e
  for(int i=0; i<len-1; i++){
    Wire.write(0xc0);                // Co=1,A0=1. data= Co-A0-0-0-0-0-0-0. 
    Wire.write(str[i]);              // sends command.
  }
  Wire.write(0x40);                  // Co=0,A0=1. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(str[len-1]);            // sends command.
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * read one byte RAM data to MCU.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
int lcd::ReadByte_dat(int col, int page){
  int dat;
  WriteByte_command(0xb0 + page);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + col/16);   //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(col%16);          //x, column address x=0-0-0-0-0-x3-x2-x1-x0 

  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  return dat;
}

/******************************************
 * display one pixel. Read-modify-Write command on page 42 of ST7567A datasheet.
 * x=0-128, y=0-31
*******************************************/
void lcd::DisplayPixel(int x,int y){
  int dat;
  WriteByte_command(0xb0 + y/8);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + x/16);    //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(x%16);           //x, column address x=0-0-0-0-0-x3-x2-x1-x0 
  
  WriteByte_command(RMW);            //start Read-modify-Write
  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  dat = (dat | (1<<(y%8)));
  WriteByte_dat(dat);                //row=bit0--bit7
  WriteByte_command(END);            //end Read-modify-Write
}

/******************************************
 * Does not display a pixel. Read-modify-Write command on page 42 of ST7567A datasheet.
 * x=0-128, y=0-31
*******************************************/
void lcd::ClearPixel(int x,int y){
  int dat;
  WriteByte_command(0xb0 + y/8);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + x/16);    //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(x%16);           //x, column address x=0-0-0-0-0-x3-x2-x1-x0 
  
  WriteByte_command(RMW);            //start Read-modify-Write
  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  //Serial.println(dat);             // print the data
  dat = (dat & (0xfe<<(y%8)));
  WriteByte_dat(dat);                //row=bit0--bit7
  WriteByte_command(END);            //end Read-modify-Write 
}

/******************************************
 * clear screen, all pixel off.
 * screen size: 128*32 dot
*******************************************/
void lcd::Clear(){
  for(int x=0; x<4; x++){
    WriteByte_command(0xb0 + x);   //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows
    
    /*******automatically increased by one******/
    WriteByte_command(0x10);       //x, column address x=0-0-0-0-1-x7-x6-x5-x4
    WriteByte_command(0x00);       //x, column address x=0-0-0-0-0-x3-x2-x1-x0
    
    for(int i=0; i<128; i++){
      WriteByte_dat(0x00);         //row=bit0--bit7
      }  
  }
}
///////////////////reserve///////////////////
void lcd::FontSize(int num){
  
}

/******************************************
 * Character display position. x=0-3, y=0-17
*******************************************/
void lcd::Cursor(int y,int x){
  if(x>17){x=17;}
  if(y>3){x=3;}
  cursor[0]=y;
  cursor[1]=x;
}

/******************************************
 * display picture.
*******************************************/
void lcd::DisplayPicture(){
  for(int x=0; x<4; x++){
    WriteByte_command(0xb0 + x);   //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows
    
    /*******automatically increased by one******/
    WriteByte_command(0x10);       //x, column address x=0-0-0-0-1-x7-x6-x5-x4
    WriteByte_command(0x00);       //x, column address x=0-0-0-0-0-x3-x2-x1-x0
    
    for(int i=0; i<128; i++){
      WriteByte_dat(pgm_read_word_near(picture+i+x*128)); 
      }  
  }
}

/******************************************
 * Writes the data from the font.c file to RAM.
*******************************************/
void lcd::WriteFont(int num){
  for(int i=0; i<7; i++){
    //reference: https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
    WriteByte_dat(pgm_read_word_near(font_7x8[num]+i));   
    //Serial.println(pgm_read_word_near(font_7x8[num]+i),HEX); 
    }
}

/******************************************
 * display font.
*******************************************/
void lcd::Display(char *str){
  int len = 0;
  len = strlen(str);
  
  WriteByte_command(0xb0 + cursor[0]);           //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows
  /*******automatically increased by one******/
  WriteByte_command(0x10 + cursor[1]*7/16);      //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(0x00 + cursor[1]*7%16);      //x, column address x=0-0-0-0-0-x3-x2-x1-x0

  for(int num=0; num<len; num++){ 
    switch(str[num]){
      case '0': WriteFont(0); break;
      case '1': WriteFont(1); break;
      case '2': WriteFont(2); break;
      case '3': WriteFont(3); break;
      case '4': WriteFont(4); break;
      case '5': WriteFont(5); break;
      case '6': WriteFont(6); break;
      case '7': WriteFont(7); break;
      case '8': WriteFont(8); break;
      case '9': WriteFont(9); break;
      case 'a': WriteFont(10); break;
      case 'b': WriteFont(11); break;
      case 'c': WriteFont(12); break;
      case 'd': WriteFont(13); break;
      case 'e': WriteFont(14); break;
      case 'f': WriteFont(15); break;
      case 'g': WriteFont(16); break;
      case 'h': WriteFont(17); break;
      case 'i': WriteFont(18); break;
      case 'j': WriteFont(19); break;
      case 'k': WriteFont(20); break;
      case 'l': WriteFont(21); break;
      case 'm': WriteFont(22); break;
      case 'n': WriteFont(23); break;
      case 'o': WriteFont(24); break;
      case 'p': WriteFont(25); break;
      case 'q': WriteFont(26); break;
      case 'r': WriteFont(27); break;
      case 's': WriteFont(28); break;
      case 't': WriteFont(29); break;
      case 'u': WriteFont(30); break;
      case 'v': WriteFont(31); break;
      case 'w': WriteFont(32); break;
      case 'x': WriteFont(33); break;
      case 'y': WriteFont(34); break;
      case 'z': WriteFont(35); break;
      case 'A': WriteFont(36); break;
      case 'B': WriteFont(37); break;
      case 'C': WriteFont(38); break;
      case 'D': WriteFont(39); break;
      case 'E': WriteFont(40); break;
      case 'F': WriteFont(41); break;
      case 'G': WriteFont(42); break;
      case 'H': WriteFont(43); break;
      case 'I': WriteFont(44); break;
      case 'J': WriteFont(45); break;
      case 'K': WriteFont(46); break;
      case 'L': WriteFont(47); break;
      case 'M': WriteFont(48); break;
      case 'N': WriteFont(49); break;
      case 'O': WriteFont(50); break;
      case 'P': WriteFont(51); break;
      case 'Q': WriteFont(52); break;
      case 'R': WriteFont(53); break;
      case 'S': WriteFont(54); break;
      case 'T': WriteFont(55); break;
      case 'U': WriteFont(56); break;
      case 'V': WriteFont(57); break;
      case 'W': WriteFont(58); break;
      case 'X': WriteFont(59); break;
      case 'Y': WriteFont(60); break;
      case 'Z': WriteFont(61); break;
      case '!': WriteFont(62); break;
      case '"': WriteFont(63); break;
      case '#': WriteFont(64); break;
      case '$': WriteFont(65); break;
      case '%': WriteFont(66); break;
      case '&': WriteFont(67); break;
      case '\'': WriteFont(68); break;
      case '(': WriteFont(69); break;
      case ')': WriteFont(70); break;
      case '*': WriteFont(71); break;
      case '+': WriteFont(72); break;
      case ',': WriteFont(73); break;
      case '-': WriteFont(74); break;
      case '/': WriteFont(75); break;
      case ':': WriteFont(76); break;
      case ';': WriteFont(77); break;
      case '<': WriteFont(78); break;
      case '=': WriteFont(79); break;
      case '>': WriteFont(80); break;
      case '?': WriteFont(81); break;
      case '@': WriteFont(82); break;
      case '{': WriteFont(83); break;
      case '|': WriteFont(84); break;
      case '}': WriteFont(85); break;
      case '~': WriteFont(86); break;
      case ' ': WriteFont(87); break;
      case '.': WriteFont(88); break;
      case '^': WriteFont(89); break;
      case '_': WriteFont(90); break;
      case '`': WriteFont(91); break;
      case '[': WriteFont(92); break;
      case '\\': WriteFont(93); break;
      case ']': WriteFont(94); break;
      default: break;
    }  
  }
}
