//
// Larry's TM1638 demo program
// Copyright (c) 2018 BitBank Software, Inc.
// Project started 5/11/2018
// written by Larry Bank (bitbank@pobox.com)
//
// This demo program implements a reaction time game. The LEDs scan back and forth for a random
// amount of time, then stop. The user then presses the button under the correct LED. The time
// in milliseconds is displayed on the right 4 digits. If it's better (lower) than previous
// times, it becomes the new best score. The left 2 digits count the number of games played.
// If the wrong button is pressed, the best score is reset to 9999.
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
#include <tm1638.h>

void setup() {
  // put your setup code here, to run once:
  // tm1638Init(byte bClock, byte bData, byte bStrobe)
  tm1638Init(3, 4, 2);
  tm1638SetBrightness(1);
}
//
// Demo Game - reaction time
void loop() {
char szTemp[16];
int iGame = 0; // number of times played
int iMinTime = 9999; // number of milliseconds of best score
int i, iTime, iDelay;
byte b, bMask, bDir;

// start by seeding the random number generator with something
// that's hopefully a little random
   randomSeed(analogRead(0));
   while (1)
   {
   // Have the discrete LEDs sequence for a few seconds
   iDelay = 50 + random(100); // random number for when to start
   bMask = bDir = 1;
   for (i=0; i<iDelay; i++) // make the LEDs bounce back and forth
   {
       if (bDir)
       {
          bMask <<= 1;
          if (bMask == 0)
          {
             bMask = 0x80;
             bDir = 0;
          }
       }
       else
       {
          bMask >>= 1;
          if (bMask == 0)
          {
            bMask = 1;
            bDir = 1;
          }
       }
       delay(50);
       tm1638SetLEDs(bMask);
   }
   iTime = millis();
   b = 0;
   while (b == 0) // wait for key press
   {
      b = tm1638ReadKeys();
   }
   iTime = millis() - iTime; // total reaction time
   randomSeed(iTime);
   // see if the user pressed the right key
   if (b == bMask) // pressed the right one
   {
      sprintf(szTemp, "%02d  %04d", iGame, iTime); // show last game time
      tm1638ShowDigits(szTemp);
      delay(2000);
      if (iTime < iMinTime)
      {
        iMinTime = iTime;
      }
   }
   else // reset the score to 9999
   {
      iMinTime = 9999;
   }  
   // Display Results
   iGame++;
   sprintf(szTemp, "%02d  %04d", iGame, iMinTime);
   tm1638ShowDigits(szTemp);
   } // while
}
