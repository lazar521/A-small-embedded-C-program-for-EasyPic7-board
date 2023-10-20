# A-small-embedded-C-program-for-EasyPic7
The program measuers voltage from 2 pins 128 times per second (64 measuers for each pin).
Then it averages the values measured during the last 1000 miliseconds and displays them on an LCD screen for each pin.
The program actually measures voltage from only one pin, since EasyPic7 board cannot have both LCD screen and 2 input pins working at the same time.
(The board only allows for 2 pins OR 1 pin and an LCD screen to work at the same time)
Regardless, the program behaves like it's reading from 2 different pins, even though it's actualy the same 1 pin.
