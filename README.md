TM1638
![TM1638](/display.jpg?raw=true "TM1638")
<br>
The TM1638 from Titan Micro Electronics is a 7-segment LED controller
It features a 3-wire interface (Clock, Data, Strobe), but doesn't conform to I2C nor SPI standards
It can control up to 16 7-segment digits along with decimal points or colons as well as has
24 possible button/keyboard inputs (8 x 3 matrix)
The datasheet says that it runs on 5V with 5V logic, but I've run it on 3.3V (both Vcc and signals)
and it seems to behave correctly.
 Since the communication protocol is custom, it requires "bit-banging" of 3 IO pins to control it.

Many LED display boards sold from China use the little brother of this chip, the TM1637
The TM1638 adds the key input. A popular board to buy with this chip contains 8 digits,

<br>
This repo contains an Arduino Library + demo sketch
<br>
See the .h file for the functions and explanations.
<br>
Copyright (c) 2018 BitBank Software, Inc.<br>
Written by Larry Bank<br>
bitbank@pobox.com<br>

