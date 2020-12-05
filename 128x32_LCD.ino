/******************************
 * project: ST7567A 128*32dot LCD
 * progranmmer: Mo 
 * Version: V1.0
 * date: 2020-12-5
 * screen:
 * x y---------------------> +
 * |                    |
 * |                    |
 * |---------------------
 * | +
 * v
 * URL: https://github.com/mworkfun/ST7567A_128X32DOT_LCD
 ******************************/
#include"lcd.h"

//create an lcd object.
lcd Lcd;

/////////////////////////////////////////////////////////
void setup() {
  //Serial.begin(9600);
  Lcd.Init();
  
  Lcd.testPixel(2);                   //use to test LCD screen. Parameter is velocity.
  
  for(int a=0; a<32; a++){
  Lcd.DisplayPixel(a,a);              //display one pixel. X=0-31, Y=0-127
  delay(5);
  }
  for(int a=0; a<32; a++){
  Lcd.ClearPixel(a,a);                //Does not display a pixel. X=0-31, Y=0-127
  delay(5);
  }
  
  Lcd.Cursor(0,7);                    //Character display position. y=0-3, x=0-17
  Lcd.Display("KEYES");               //Maximun 18 characters.
  Lcd.Cursor(1,0);
  Lcd.Display("ABCDEFGHIJKLMNOPQR");
  Lcd.Cursor(2,0);
  Lcd.Display("123456789+-*/<>=$@");
  Lcd.Cursor(3,0);
  Lcd.Display("%^&(){}:;'|?,.~\\[]");
  delay(1000);
  Lcd.Clear();                        //All pixels turn off.
  
  Lcd.DisplayPicture();               //Displays the image data for the picture.c file
  delay(1000);
}

/////////////////////////////////////////////////////////
void loop() {

}
