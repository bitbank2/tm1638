#include <Arduino.h>
#include <tm1638.h>
//
// Larry's TM1638 library
// Copyright (c) 2018 BitBank Software, Inc.
// Project started 5/11/2018
// written by Larry Bank (bitbank@pobox.com)
//
// The TM1638 from Titan Micro Electronics is a 7-segment LED controller
// It features a 3-wire interface (Clock, Data, Strobe), but doesn't conform to I2C nor SPI standards
// It can control up to 16 7-segment digits along with decimal points or colons as well as has
// 24 possible button/keyboard inputs (8 x 3 matrix)
// The datasheet says that it runs on 5V with 5V logic, but I've run it on 3.3V (both Vcc and signals)
// and it seems to behave correctly.
// Since the communication protocol is custom, it requires "bit-banging" of 3 IO pins to control it.
//
// Many LED display boards sold from China use the little brother of this chip, the TM1637
// The TM1638 adds the key input. A popular board to buy with this chip contains 8 digits,
// 8 discrete LEDs and 8 push buttons. The mapping of the digits and discrete LEDs is a bit
// odd (in my opinion). They're mapped such that even bytes are mapped to the digits and bit 0
// of the odd bytes are mapped to the discrete LEDs. The keys are mapped at bit 0 and 4 of the
// 4 input bytes.
//
// The key mapping is spread out over 4 bytes. The bit 0 of each byte represents keys 1-4
// and bit 4 of each byte represents keys 5-8. For example, key "S6" is bit 4 of byte 1.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
// table which translates a digit into the segments
//
const byte cDigit2Seg[] = {0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

static byte bClockPin, bDataPin, bStrobePin;
void tm1638Clear(void);
#define CLOCK_DELAY 5
//
// Initialize the tm1638 by specifying the digital pins used to communicate
//
void tm1638Init(byte bClock, byte bData, byte bStrobe)
{
byte b;

   bClockPin = bClock;
   bDataPin = bData;
   bStrobePin = bStrobe;
   pinMode(bClockPin, OUTPUT);
   pinMode(bDataPin, OUTPUT);
   pinMode(bStrobePin, OUTPUT);
   digitalWrite(bClockPin, HIGH);
   digitalWrite(bStrobePin, HIGH);
   tm1638Clear(); // clear the memory
} /* tm1638Init() */
//
// Read a byte from the controller
//
static byte tm1638ReadByte(void)
{
byte b, i;

   b = 0;
   for (i=0; i<8; i++)
   {
      digitalWrite(bClockPin, LOW); // clock low
      if (digitalRead(bDataPin) == HIGH)
         b |= (1 << i);
      delayMicroseconds(CLOCK_DELAY);
      digitalWrite(bClockPin, HIGH); // clock high
      delayMicroseconds(CLOCK_DELAY);
   } // for i
   return b;
} /* tm1638WriteByte() */
//
// Write a byte to the controller
//
static void tm1638WriteByte(byte b)
{
byte i;

   for (i=0; i<8; i++)
   {
      digitalWrite(bClockPin, LOW); // clock low
      if (b & 1) // LSB to MSB
         digitalWrite(bDataPin, HIGH);
      else
         digitalWrite(bDataPin, LOW);
      delayMicroseconds(CLOCK_DELAY);
      digitalWrite(bClockPin, HIGH); // clock high
      delayMicroseconds(CLOCK_DELAY);
      b >>= 1;
   }
} /* tm1638WriteByte() */
//
// Write a sequence of bytes to the controller
//
static void tm1638Write(byte *pData, byte bLen)
{
byte b;
   digitalWrite(bStrobePin, LOW);
   delayMicroseconds(CLOCK_DELAY);
//   tm1638Start();
   for (b=0; b<bLen; b++)
   {
      tm1638WriteByte(pData[b]);
      delayMicroseconds(CLOCK_DELAY);
   }
//   tm1638Stop();
   digitalWrite(bStrobePin, HIGH);
   delayMicroseconds(CLOCK_DELAY);
} /* tm1638Write() */
//
// Set brightness (0-8)
//
void tm1638SetBrightness(byte b)
{
byte bControl;
   if (b == 0) // display off
      bControl = 0x80; // display off
   else
   {
      if (b > 8) b = 8;
      bControl = 0x88 | (b - 1);
   }
   tm1638Write(&bControl, 1);
} /* tm1638SetBrightness() */
//
// Clear the internal memory (turn off all segments)
//
void tm1638Clear(void)
{
byte bTemp[17];

  bTemp[0] = 0x40; // data write mode set
  tm1638Write(bTemp, 1);

  memset(bTemp, 0, sizeof(bTemp));
  bTemp[0] = 0xc0; // set memory address to 0
  tm1638Write(bTemp, 17); // clear 16 bytes
} /* tm1638Clear() */
//
// Set the 8 discrete LEDs
// Each bit sets an LED on or off
//
void tm1638SetLEDs(byte b)
{
byte i;
byte bTemp[2];

  bTemp[0] = 0x40; // data write mode set
  tm1638Write(bTemp, 1);
  
  for (i=0; i<8; i++)
  {
    bTemp[0] = 0xc1 + i*2; // odd digits are the LEDs
    bTemp[1] = (b & (1<<i)) ? 1:0;
    tm1638Write(bTemp, 2);
  }
} /* tm1638SetLEDs() */
//
// Read the key switch status
// The bits are mapped oddly across 4 bytes
// return a single byte where each key represents 1 bit
//
byte tm1638ReadKeys(void)
{
byte b, i;

   b = 0xc0; // set the address to 0
   tm1638Write(&b, 1);

   // start the read sequence
   digitalWrite(bStrobePin, LOW);
   delayMicroseconds(CLOCK_DELAY);
   tm1638WriteByte(0x42); // send the read-key command
   pinMode(bDataPin, INPUT); // set data line as an input
   b = 0;
   for (i=0; i<4; i++) // read 4 bytes sequentially
   {
      b |= (tm1638ReadByte() << i); 
   } // for i
   digitalWrite(bStrobePin, HIGH);
   delayMicroseconds(CLOCK_DELAY);
   pinMode(bDataPin, OUTPUT);
   return b;
} /* tm1638ReadKeys() */
//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1638ShowDigits(char *pString)
{
byte bTemp[2]; // commands and data to transmit
byte i, j;

   bTemp[0] = 0x40; // data write mode set
   tm1638Write(bTemp, 1);

   for (i=0, j=0; i<strlen(pString) && j < 8; i++)
   {

//            bTemp[2] |= 0x80; // second digit high bit controls colon LEDs
      if (pString[i] >= '0' && pString[i] <= '9')
      {
         bTemp[0] = 0xc0 + j*2; // set display address for each digit (even values)
         bTemp[1] = cDigit2Seg[pString[i] & 0xf]; // segment data
         tm1638Write(bTemp, 2);
         j++;
      }
      else if (pString[i] == ' ') // blank this digit
      {
         bTemp[0] = 0xc0 + j*2; // set display address for each digit (even values)
         bTemp[1] = 0; // segment data
         tm1638Write(bTemp, 2);
         j++;
      }
      else if (pString[i] == '.' && j != 0) // add . to last digit
      {
         bTemp[1] |= 0x80; // set decimal point
         tm1638Write(bTemp, 2); // write the last digit again
      }
   }
} /* tm1638ShowDigits() */
