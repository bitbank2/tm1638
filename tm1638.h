#ifndef __TM1638__
#define __TM1638__
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
// Public functions
//
//
// Clear the internal memory (turn off all segments)
//
void tm1638Clear(void);

// Initialize the tm1638 by specifying the digital pins used to communicate
//
void tm1638Init(byte bClock, byte bData, byte bStrobe);
//
// Set brightness (0-8)
//
void tm1638SetBrightness(byte b);
//
// Set the 8 discrete LEDs
// each bit set an LED on or off
// e.g. 0x00 = all off, 0xff = all on
//
void tm1638SetLEDs(byte b);
//
// Read the key switch status
// Returns a single byte where
// each bit (set to 1 if pressed)
// represents the 8 keys
byte tm1638ReadKeys(void);

//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1638ShowDigits(char *pString);

#endif // __TM1638__
