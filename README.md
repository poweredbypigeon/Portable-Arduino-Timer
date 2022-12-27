# Portable-Arduino-Timer
A portable Arduino timer. Has two different modes: one where it counts up, and one where it counts down. The user interface is done through an LCD screen, and the input comes from 2 push buttons.

**MODE 1:**

The timer starts counting up. You can press the first button to alternately pause/resume the timer, and hold down the 2nd button to leave. There will also be a speaker that beeps every minute to alert you.

**MODE 2:**

First, you select how much time you wnat to give yourself (e.g. a 15 min, 30 min timer), and hold down both buttons to start the actual timer. There will be an RGB LED that changes colour as the timer counts down so you can measure progress (this was actually one of my favourite parts of building the project).

I ended up changing the code slightly as the physical LCD I was using required a different module (LiquidCrystal_I2C.h) instead of TinkerCAD's LiquidCrystal.h. If you want to run this code on your own, then you will need to install the LiquidCrystal_I2C.h here: https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
