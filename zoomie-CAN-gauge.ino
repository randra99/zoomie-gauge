#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "CAN.h"
#include <math.h>

// Define pins for display interface. You'll probably need to edit this for
// your own needs:

#if defined(ARDUINO_SEEED_XIAO_RP2040)

// Pinout when using Seed Round Display for XIAO in combination with
// Seeed XIAO RP2040. Other (non-RP2040) XIAO boards, any Adafruit Qt Py
// boards, and other GC9A01A display breakouts will require different pins.
#define TFT_CS D1 // Chip select
#define TFT_DC D3 // Data/command
#define TFT_BL D6 // Backlight control

#else // ALL OTHER BOARDS - EDIT AS NEEDED

// Other RP2040-based boards might not have "D" pin defines as shown above
// and will use GPIO bit numbers. On non-RP2040 boards, you can usually use
// pin numbers silkscreened on the board.
#define TFT_DC  7
#define TFT_CS 10
// If display breakout has a backlight control pin, that can be defined here
// as TFT_BL. On some breakouts it's not needed, backlight is always on.

#endif

// Display constructor for primary hardware SPI connection -- the specific
// pins used for writing to the display are unique to each board and are not
// negotiable. "Soft" SPI (using any pins) is an option but performance is
// reduced; it's rarely used, see header file for syntax if needed.
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

int water_temp = 0;
int oil_pressure = 0;
int oil_temp = 0;
int rpm = 0;

int last_water_temp = -1;
int last_oil_pressure = -1;
int last_oil_temp = -1;

int no_water_temp = 0;
bool no_rpm = true;
bool warning_on = false;

void write_num(int x, int y, int size, int num, int colour=WHITE)
{
  if(num < 0)
  {
    x=x-15;
    if(num < -99)
      x=x-15;
    if(num > -10)
      x=x+15;
  }
  else
  {
    if(num < 10)
      x=x+15;
    if(num > 99)
      x=x-15;
  }
  tft.setTextColor(colour);
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.println(String(num));
}

void write_none(int x, int y, int size, int colour=WHITE)
{
  tft.setTextColor(colour);
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.println("XX");
}

void write_values(int water_temp, int oil_pressure, int oil_temp, bool force_write=false)
{
  if(last_water_temp!=water_temp || force_write)
  {
    tft.fillRect(92, 13, 70, 35, BLACK);
    write_num(92,13,5,water_temp);
    last_water_temp = water_temp;
  }
  if(last_oil_pressure!=oil_pressure || force_write)
  {
    tft.fillRect(125, 102, 70, 35, BLACK);
    write_num(125,102,5,oil_pressure);
    last_oil_pressure = oil_pressure;
  }
  if(last_oil_temp!=oil_temp || force_write)
  {
    tft.fillRect(90, 188, 70, 35, BLACK);
    write_num(90,188,5,oil_temp);
    last_oil_temp = oil_temp;
  }
}

void write_text(int x, int y, int size, char* text, int colour=WHITE)
{
  tft.setTextColor(colour);
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.write(text);
}

void display_water_warning()
{
  tft.fillScreen(RED);
  write_text(50,60,5,"WATER");
  write_text(65,100,5,"TEMP");
  write_num(90,150,5,water_temp);
  delay(200);
  tft.fillScreen(WHITE);
  write_text(50,60,5,"WATER", BLACK);
  write_text(65,100,5,"TEMP", BLACK);
  write_num(90,150,5,water_temp, BLACK);
  delay(200);
}

void display_no_water_temp_warning()
{
  tft.fillScreen(RED);
  write_text(90,60,5,"NO");
  write_text(50,100,5,"WATER");
  write_text(65,150,5,"TEMP");
  delay(200);
  tft.fillScreen(WHITE);
  write_text(90,60,5,"NO", BLACK);
  write_text(50,100,5,"WATER", BLACK);
  write_text(65,150,5,"TEMP", BLACK);
  delay(200);
}

void display_oil_press_warning()
{
  tft.fillScreen(RED);
  write_text(80,60,5,"OIL");
  write_text(50,100,5,"PRESS");
  write_num(90,150,5,oil_pressure);
  delay(200);
  tft.fillScreen(WHITE);
  write_text(80,60,5,"OIL", BLACK);
  write_text(50,100,5,"PRESS", BLACK);
  write_num(105,150,5,oil_pressure, BLACK);
  delay(200);
}

void display_oil_temp_warning()
{
  tft.fillScreen(RED);
  write_text(80,60,5,"OIL");
  write_text(60,100,5,"TEMP");
  write_num(90,150,5,oil_temp);
  delay(200);
  tft.fillScreen(WHITE);
  write_text(80,60,5,"OIL", BLACK);
  write_text(60,100,5,"TEMP", BLACK);
  write_num(90,150,5,oil_temp, BLACK);
  delay(200);
}

void draw_info_screen()
{
  tft.fillScreen(BLACK);

  tft.drawCircle(120,120, 119, RED);
  write_text(60,60,2,"WATER TEMP");
  tft.drawLine(0, 85, 240, 85, WHITE);
  write_text(10,100,2,"OIL");
  write_text(10,120,2,"PRESS");
  tft.drawLine(0, 155, 250, 155, WHITE);
  write_text(75,162,2,"OIL TEMP");
  write_values(water_temp,oil_pressure,oil_temp,true);
}

void screen_setup()
{
  tft.begin();

  tft.setRotation(1);

  #if defined(TFT_BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Backlight on
  #endif // end TFT_BL

  draw_info_screen();
}

void setup() {
  screen_setup();

  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  CAN.setClockFrequency(8E6);
  // start the CAN bus at 500 kbps
  while(!CAN.begin(500E3)) {
    tft.fillScreen(WHITE);
    Serial.println("Starting CAN failed!");
    tft.fillScreen(CYAN);
  }
}

void read_bosch_sensor()
{
  int raw = 0;
  int Vin = 5;
  float Vout = 0;
  float R1 = 4700;
  float R2 = 0;
  float buffer = 0;

  float pressure_voltage = analogRead(A0) * 5 / 1024.0;
  oil_pressure = round(36.25 * pressure_voltage - 18.125);
  Serial.print("Pressure :");
  Serial.println(oil_pressure);

  raw = analogRead(A1);
  if(raw){
    R2 = R1 / ((Vin / ( raw * 5 / 1024.0) ) - 1);
    oil_temp = round( 279 -33.9 * log(R2));

    Serial.print("Temp: ");
    Serial.println(oil_temp);

  }
}

void read_can_water_temp()
{
  int packetSize;
  unsigned long StartTime = millis();
  unsigned long CurrentTime = millis();
  do{
    packetSize = CAN.parsePacket();
    CurrentTime = millis();
  }while(!(packetSize && CAN.packetId()==0x240) && (CurrentTime - StartTime < 1000));

  if(CAN.packetId()==0x240)
  {
    int byteno = 0;
    int len = CAN.available();
    for(int i = 0; i < len; i++)
    {
      if(i == 1)
      {
          water_temp = CAN.read()-40;
          Serial.println("Water temp: " + water_temp);
          write_values(water_temp,oil_pressure,oil_temp);
      }
      else
        CAN.read();
    }
    no_water_temp = 0;
  }
  else
    no_water_temp++;
}

void read_can_rpm()
{
  int packetSize;
  unsigned long StartTime = millis();
  unsigned long CurrentTime = millis();
  do{
    packetSize = CAN.parsePacket();
    CurrentTime = millis();
  }while(!(packetSize && CAN.packetId()==0x201) && (CurrentTime - StartTime < 1000));

  if(CAN.packetId()==0x201)
  {
    byte rpm_byte0, rpm_byte1;
    int byteno = 0;
    int len = CAN.available();
    for(int i = 0; i < len; i++)
    {
      if(i == 0)
          rpm_byte0 = CAN.read();
      if(i == 1)
          rpm_byte1 = CAN.read();
      else
        CAN.read();

      rpm = ((rpm_byte0 << 8) + rpm_byte1) / 4;      
      Serial.print("RPM:");
      Serial.println(rpm);  
    }
    no_rpm = false;
  }
  else
    no_rpm = true;
}

void loop(void) {
  read_can_water_temp();
  read_can_rpm();
  read_bosch_sensor();
  delay(100);

  if(((no_rpm || (rpm > 0 && rpm < 1000)) && oil_pressure < 10) || (rpm > 3000 && oil_pressure < 20))
  {
    display_oil_press_warning();
    warning_on = true;
  }
  else if(oil_temp > 120 || oil_temp < -100)
  {
    display_oil_temp_warning();
    warning_on = true;
  }
  else if(no_water_temp > 10)
  {
    display_no_water_temp_warning();
    warning_on = true;
  }
  else if(water_temp > 115)
  {
    display_water_warning();
    warning_on = true;
  }
  else
  {
    if(warning_on)
    {
      draw_info_screen();
      warning_on = false;
    }
    else
    {
      write_values(water_temp,oil_pressure,oil_temp);
    }
  }
}