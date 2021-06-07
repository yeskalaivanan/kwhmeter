#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>//Display
#include <SPI.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
int total_meters = 28;

String DisplayString = "Welcome to VEPL";

int addr = 1;

//int count_address = 1;
int count_address[] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60};
//int kwh_resolution_address = 10;
int kwh_whole_address[] = {62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120};
//int kwh_total_address = 30;

const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;

int count[30];
float kwh_resolution[30];
int kwh_whole[30];
float kwh_total[30];

bool Previous_state[30], intterupt_occured[30];

int Energy_Pulse[30];
//float Energy_Previous = 0.0;
int myPins[] = {2,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49};
int value[30];
int i=0;
unsigned int time_count = 0;
void setup() 
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(DisplayString);
  
  pinMode(ledPin, OUTPUT);  
  Serial.begin(9600);//Serial start with 9600

  for(i=0;i<=(total_meters-1);i++)
  {
    pinMode(myPins[i], INPUT_PULLUP);
    count[i] = readIntFromEEPROM(count_address[i]);
    kwh_whole[i] = readIntFromEEPROM(kwh_whole_address[i]);
    calculation(i);
    delay(100);
  }   
}

void loop() 
{
    //Starting to counting pulses
    for(i=0;i<=(total_meters-1);i++)
    {
      //Read pin status
      value[i] = digitalRead(myPins[i]);
      
      if (value[i] == 1 && Previous_state[i] == 0)
      {
        intterupt_occured[i] = 1;
        count[i]++;
        Energy_Pulse[i]++;  
        Previous_state[i] = 1;    
      }//if (value[i] == 1 && Previous_state[i] == 0)
      
      if(value[i] == 0)
      {
        Previous_state[i] = 0;
      }//if(value[i] == 0)
      ///////////////////////////////////////
      if(intterupt_occured[i] == 1)
      {
        intterupt_occured[i] = 0;        
        calculation(i);//kwh calculation
        //Backup for every 100 pulse/ 0.1 Kwh
        if(Energy_Pulse[i] >= 100)
        {
          Energy_Pulse[i] = 0;
          updateEEPROM(i);//Write/Update values in EEPROM 
        }//if(Energy_Pulse[i] >= 100)        
        //LCD_Update(i);
        Print_Serial(i);
      }//if(intterupt_occured[i] == 1)           
    } //for(i=0;i<=(total_meters-1);i++)         
}//void
void LCD_Update(int No)
{
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Pulse : ");    
  lcd.print(count[No]);
  lcd.setCursor(0, 1); 
  lcd.print("Energy : ");    
  lcd.print(kwh_total[No]);
}//void LCD_Update(int No)

void Print_Serial(int No)
{
  Serial.print(No);
  Serial.print(",");
  Serial.println(kwh_total[No]);
}//void Print_Serial(int No)
void calculation(int No)
{
  if(count[No] >= 1000)
  {
    kwh_whole[No] = kwh_whole[No] + 1;
    count[No] = count[No] - 1000;
    updateEEPROM(No);//Write/Update values in EEPROM
  }
  
  kwh_resolution[No] = count[No] / 1000.0;  
  kwh_total[No] = kwh_whole[No] + kwh_resolution[No];
}//void calculation(int No)

void updateEEPROM(int No)
{  
    writeIntIntoEEPROM(count_address[No],count[No]);
    writeIntIntoEEPROM(kwh_whole_address[No],kwh_whole[No]);
}//void updateEEPROM(int No)

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

/*int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}*/

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}//int readIntFromEEPROM(int address)

void writeLongIntoEEPROM(int address, long number)
{ 
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}//void writeLongIntoEEPROM(int address, long number)

long readLongFromEEPROM(int address)
{
  return ((long)EEPROM.read(address) << 24) +
         ((long)EEPROM.read(address + 1) << 16) +
         ((long)EEPROM.read(address + 2) << 8) +
         (long)EEPROM.read(address + 3);
}//long readLongFromEEPROM(int address)
