// Jennifer AUBINAIS

#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 // Optinal

#define KS 48 // Keyboard Size

#define	BLACK   0x0000 // 0
#define BROWN   0x9980 // 1
#define	RED     0xF800 // 2
#define ORANGE  0xFB00 // 3
#define YELLOW  0xFFE0 // 4
#define GREEN   0x03E0 // 5
#define BLUE    0x101F // 6
#define PURPLE  0x800F // 7
#define GRAY    0x94D2 // 8
#define WHITE   0xFFFF // 9


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int X, Y, Z;
enum listFunc { NONE, F_HEX, F_DEC, F_BIN, F_RES }; 
byte Function = NONE;
String valInput = "";

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("Hello, Calculator"));
  
  tft.reset();
  
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    identifier=0x9341;
  }

  tft.begin(identifier);
  tft.setRotation(0);
  tft.fillRect(0, 0, 240, 320, BLACK);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  displayKey(0, 3, BROWN, WHITE, " 1 ");
  displayKey(0, 4, YELLOW, WHITE, " 4 ");
  displayKey(0, 5, PURPLE, WHITE, " 7 ");
  displayKey(0, 6, BLACK, WHITE, " 0 ");
  displayKey(1, 3, RED, WHITE, " 2 ");
  displayKey(1, 4, GREEN, WHITE, " 5 ");
  displayKey(1, 5, GRAY, WHITE, " 8 ");
  displayKey(1, 6, BLACK, WHITE, " E ");
  displayKey(2, 3, ORANGE, WHITE, " 3 ");
  displayKey(2, 4, BLUE, WHITE, " 6 ");
  displayKey(2, 5, WHITE, BLACK, " 9 ");
  displayKey(2, 6, BLACK, WHITE, " F "); 
  displayKey(3, 3, BLACK, WHITE, " A ");
  displayKey(3, 4, BLACK, WHITE, " B ");
  displayKey(3, 5, BLACK, WHITE, " C ");
  displayKey(3, 6, BLACK, WHITE, " D ");
  displayKey(4, 3, BLACK, WHITE, "HEX");
  displayKey(4, 4, BLACK, WHITE, "DEC");
  displayKey(4, 5, BLACK, WHITE, "BIN");
  displayKey(4, 6, BLACK, WHITE, "Res");
  for(byte x = 0; x < 5; x++)
  {
    tft.drawLine(x*KS, 317, x*KS, 317 - 4*KS, WHITE);
    tft.drawLine(x*KS + 1, 317, x*KS + 1, 317 - 4*KS, WHITE);
  }
  tft.drawLine(5*KS - 2, 317, 5*KS - 2, 317 - 4*KS, WHITE);
  tft.drawLine(5*KS - 1, 317, 5*KS - 1, 317 - 4*KS, WHITE);
  for(byte y = 0; y < 5; y++)
  {
    tft.drawLine(0, 316 - y*KS, 240, 316 - y*KS, WHITE);
    tft.drawLine(0, 317 - y*KS, 240, 317 - y*KS, WHITE);
  }
  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000


/******************************************************************************************/
/******************************************************************************************/
void loop()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    byte valKey = valueKey(p);
    if (valKey >= 100)
    {
      if (Function == NONE)
      {
        tft.setCursor(10, 10);
        tft.setTextColor(BLACK,WHITE);
        switch(valKey)
        {
          case 103:
            tft.println(" HEX ");
            displayKey(4, 3, WHITE, BLACK, "HEX");
            Function = F_HEX;
            break;
          case 102:
            tft.println(" DEC ");
            displayKey(4, 4, WHITE, BLACK, "DEC");
            Function = F_DEC;
            break;
          case 101:
            tft.println(" BIN ");
            displayKey(4, 5, WHITE, BLACK, "BIN");
            Function = F_BIN;
            break;
          case 100:
            tft.println(" Resistor ");
            displayKey(4, 6, WHITE, BLACK,  "Res");
            Function = F_RES;
            break;
          default:
            break;
        }
      }
      else
      {

      }
    }
    else
    {
      if (Function != NONE)
      {
        if (valKey < 10)
        {
          valInput = valInput + char(48 + valKey);
        }
        else
        {
          valInput = valInput + char(55 + valKey);
        }
        tft.setCursor(10, 30);
        tft.setTextColor(WHITE);
        tft.println(valInput);
      }
    }
  }
}
/******************************************************************************************/
/******************************************************************************************/
byte valueKey(TSPoint p)
{
    byte Val = 255;
    switch(p.x)
    {
      case 192 ... 240:
        switch(p.y)
        {
          case 0 ... 47:
            Val = 0;break;
          case 48 ... 95:
            Val = 7;break;
          case 96 ... 143:
            Val = 4;break;
          case 144 ... 192:
            Val = 1;break;
          default:
            break;
        }
        break;
      case 144 ... 191:
        switch(p.y)
        {
          case 0 ... 47:
            Val = 14;break;
          case 48 ... 95:
            Val = 8;break;
          case 96 ... 143:
            Val = 5;break;
          case 144 ... 192:
            Val = 2;break;
          default:
            break;
        }
        break;
      case 96 ... 143:
        switch(p.y)
        {
          case 0 ... 47:
            Val = 15;break;
          case 48 ... 95:
            Val = 9;break;
          case 96 ... 143:
            Val = 6;break;
          case 144 ... 192:
            Val = 3;break;
          default:
            break;
        }
        break;
      case 48 ... 95:
        switch(p.y)
        {
          case 0 ... 47:
            Val = 13;break;
          case 48 ... 95:
            Val = 12;break;
          case 96 ... 143:
            Val = 11;break;
          case 144 ... 192:
            Val = 10;break;
          default:
            break;
        }
        break;
      case 0 ... 47:
        switch(p.y)
        {
          case 0 ... 47:
            Val = 100;break; // Res
          case 48 ... 95:
            Val = 101;break; // BIN
          case 96 ... 143:
            Val = 102;break; // DEC
          case 144 ... 192:
            Val = 103;break; // HEX
          default:
            break;
        }
        break;
      default:
        break;
    }
    Serial.println(Val);
    delay(300);
    return Val;
}
/******************************************************************************************/
/******************************************************************************************/
void displayKey(byte x, byte y, uint16_t colKey, uint16_t colText, String st)
{
  tft.fillRect(x*KS, y*KS - 20, KS, KS, colKey);
  tft.setCursor(7 + x*48, y*48 - 5);
  tft.setTextColor(colText);
  tft.println(st);
}
/******************************************************************************************/
/******************************************************************************************/
uint16_t rgb565(uint16_t R, uint16_t G, uint16_t B)
{
  uint16_t ret  = (R & 0xF8) << 8;  // 5 bits
           ret |= (G & 0xFC) << 3;  // 6 bits
           ret |= (B & 0xF8) >> 3;  // 5 bits     
  return( ret);
}
