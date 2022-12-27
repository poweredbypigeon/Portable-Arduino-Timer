#include <LiquidCrystal.h>
LiquidCrystal LCD(8, 9, 10, 11, 12, 13); 


const int btn1 = 2;
const int btn2 = 4; 

const int redLED = 3;
const int blueLED = 5;
const int greenLED = 6; // all PWM ports 

const int piezo = A1;

int state1 = 0;
int state2 = 0;

int stateTotal = 0;

int updateCycle = 0; // between 0, 1, 2, and 3. Always increments every 250 ms and timer updates if it is 0. basically a helper variable for time

// general

int screen = 1;

// time

float t = 0.0; // rgb progress

int startTime = millis(); // the time in which you pressed the timer

long timer = 0; // time that appears

bool paused = false; 

int timeSinceStopPressed = 0;

// acceleration kit. 

int lastState = 0;
int totalTime = 0;
int timeSinceLastChange = 0; 
int delayTime = 250; 
const int MS_ACCELERATION = 10; 
/* bring it down faster, acceleration starts after two seconds of holding it down. This feature was borrowed off of Samsung */
const int MIN_DELAY = 100;
const int MAX_DELAY = 250; 

const int SLOW_BTN = 5;
const int FAST_BTN = 60; 


const int TIME_LIMIT = 86400; 

int counter = 60;


const float x = 255/60; 

float period = 60.0; // in seconds 

void setcolor (int red, int green, int blue) { // something wrong with set color function. (255, 0, 0) turns to (0, 0, 255). 
	analogWrite(redLED, red);
  	analogWrite(greenLED, green);
  	analogWrite(blueLED, blue);
}

void RGB(float H) { // 0 <= H <= 360
  int r = 0;
  int g = 0;
  int b = 0;
  if (H <= 60) {
  	r = 255;
    g = H*x;
    b = 0;
  } else if (H <= 120) {
  	r = 255 - (H-60)*x;
    g = 255;
    b = 0;
  } else if (H <= 180) {
  	r = 0;
    g = 255;
    b = (H-120)*x;
  } else if (H <= 240) {
  	r = 0;
    g = 255 - (H-180)*x;
    b = 255; 
  } else if (H <= 300) {
  	r = (H-240)*x;
    g = 0;
    b = 255;
  } else if (H <= 360) {
  	r = 255;
    g = 0;
    b = 255 - (H-300)*x;
  }
  setcolor(r, g, b);
}

// LOW LOW LOW turns it off

int returnState(int s1, int s2) { // safeguard against people pressing both at the same time.
  if (s1 == HIGH and s2 == LOW) {
  	return 1;
  } else if (s1 == LOW and s2 == HIGH) {
  	return 2; 
  } else if (s1 == HIGH and s2 == HIGH) {
  	return 3; // added this
  } else {
  	return 0; 
  }
}



String formatTime(long x) {
  String output = "";
  if (x >= 3600) {
  	int hrs = (x / 3600);
    if (hrs < 10) {
    	output += "0";
    }
    output += hrs;
  	x = x % 3600;
    output += ":";
  }
  int mins = (x / 60);
  if (mins < 10) {
  	output += "0";
  }
  output += mins;
  output += ":";
  x = x % 60;
  if (x < 10) {
  	output += "0";
  }
  output += x;
  return output;
}

// formats time into hours

void setup()
{
  Serial.begin(9600);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT); 
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(piezo, OUTPUT);
  LCD.begin(16, 2);
  LCD.setCursor(0, 0);
  LCD.print("PORTABLE TIMER");
  delay(1000);
  tone(piezo, 1000, 500);
  tone(piezo, 800, 500);
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("A PIGEON MEDIA");
  LCD.setCursor(0, 1);
  LCD.print("PRODUCTION");
  delay(1000);
  tone(piezo, 800, 500);
  tone(piezo, 1000, 500);
  LCD.setCursor(0, 0);
  // screen = 3; // debug moment , debug only
}

void loop() 
{
  totalTime = millis(); 
  state1 = digitalRead(btn1);
  state2 = digitalRead(btn2); 
  lastState = stateTotal; // variable purely for acceleration
  stateTotal = returnState(state1, state2);
  //Serial.println(stateTotal);
  if (screen == 1) {
    // clean variables here, except for counter... 
    timer = 0;
    updateCycle = 0;
    paused = false;
    timeSinceStopPressed = 0;
    delayTime = MAX_DELAY;
    noTone(piezo);
    setcolor(LOW, LOW, LOW);
    // end clean
  	LCD.clear();
    LCD.setCursor(0, 0); 
    LCD.print("P1: Count Up");
    LCD.setCursor(0, 1);
    LCD.print("P2: Count Down");
    if (stateTotal == 1) {
      	screen = 2;
      	LCD.clear();
    	LCD.setCursor(0, 0);
        LCD.print("Loading...");
      	delay(2000); 
    } else if (stateTotal == 2) {
      	screen = 3; 
      	LCD.clear();
      	LCD.setCursor(0, 0);
        LCD.print("Loading...");
      	delay(2000); 
    }
  } else if (screen == 2) {
    // display
    if (updateCycle == 2) { // lcd was flashing too much, only clears when necessary
    	LCD.clear();
    }
    
    if (paused) {
      	updateCycle = 0;
      	LCD.setCursor(0, 1);
      	LCD.print("P1:PLAY  P2:EXIT");
    } else {
    	LCD.setCursor(0, 1);
      	LCD.print("P1:PAUSE P2:EXIT");
      	updateCycle += 1;
      	updateCycle = updateCycle % 4; 
        if (updateCycle == 3) {
          timer += 1; 
        }
    }
    LCD.setCursor(0, 0); 
   	LCD.print("TIME: ");
    LCD.print(formatTime(timer));
    if (timer >= 86400) { 
      	delay(3000);
      	LCD.clear();
    	LCD.setCursor(0, 0);
      	LCD.print("TLE");
      	LCD.setCursor(0, 1);
      	LCD.print("Returning home");
      	delay(2000);
      	screen = 1;
    }
    
    // receive input
    if (stateTotal == 1) {
    	paused = !paused;
    } else if (stateTotal == 2) {
      if (lastState != 2) {
      	timeSinceStopPressed = millis();
      } else {
        if (totalTime - timeSinceStopPressed >= 2000) {
          	setcolor(LOW, LOW, LOW);
        	tone(piezo, 800, 250);
          	delay(500);
          	tone(piezo, 800, 250);
          	screen = 1;
          	LCD.clear();
            LCD.setCursor(0, 0); 
            LCD.print("Returning to");
          	LCD.setCursor(0, 1);
          	LCD.print("home screen..."); 
            delay(3000); // can't have anyone pressing this too soon because they will immediately select count down
        } else {
        	int progress = (totalTime - timeSinceStopPressed);
          	float coeff = ((float) progress) * 255.0 / 2000.0;
          	int g = 255 - coeff; // integer multiplication is scuffed, it thinks 1871 * 255 = 18353
          	Serial.print(progress);
          	Serial.print(" ");
          	Serial.println(g);
          	setcolor(255, g, g);
        }
      }
    }
  	// can't use millis here. Have a delay 250 and a counter that hits every 4 cycles.
  } else if (screen == 3) {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("TIME: ");
    LCD.print(formatTime(counter));
    LCD.setCursor(0, 1);
    LCD.print("Hold both- start");
    
  	if (stateTotal == 1) {
      counter += 10;
      if (counter > 86400) {
      	counter = 86400;
      }
    } else if (stateTotal == 2) {
      counter -= 10;
      if (counter < 10) {
      	counter = 10;
      }
    }
    if (lastState == stateTotal) {
      if ((stateTotal == 3) and (totalTime - timeSinceLastChange >= 3000)) {
      	timer = counter;
        screen = 4; 
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("Loading...");
        delay(2000); 
      }
      if (totalTime - timeSinceLastChange >= 2000) {
        // RGB light as progres
          delayTime -= MS_ACCELERATION; 
          if (delayTime < MIN_DELAY) {
            delayTime = MIN_DELAY;
          }
      }
    } else {
      timeSinceLastChange = millis();
      delayTime = MAX_DELAY;
    } 
  } else if (screen == 4) {
  	// display
    
    if (updateCycle == 2) { // lcd was flashing too much, only clears when necessary
    	LCD.clear();
    }
    if (paused) {
      	updateCycle = 0;
      	LCD.setCursor(0, 1);
      	LCD.print("P1:PLAY  P2:EXIT");
    } else {
    	LCD.setCursor(0, 1);
      	LCD.print("P1:PAUSE P2:EXIT");
      	updateCycle += 1;
      	updateCycle = updateCycle % 4; 
        if (updateCycle == 3) {
          timer -= 1; 
        }
    }
    LCD.setCursor(0, 0);
   	LCD.print("TIME: ");
    LCD.print(formatTime(timer));
    t = (360.0 / counter) * (counter - timer); 
	RGB(t);
    // update
    if (timer == 0) {
      	delay(1000);
    	tone(piezo, 1500, 2000); 
      	LCD.clear();
      	LCD.setCursor(0, 0);
      	LCD.print("Time's up");
      	LCD.setCursor(0, 1);
      	LCD.print("Returning home");
      	screen = 1; 
      	delay(3000); 
    }
    // receive input
    
    if (stateTotal == 1) {
    	paused = !paused;
    } else if (stateTotal == 2) {
      if (lastState != 2) {
      	timeSinceStopPressed = millis();
      } else {
        if (totalTime - timeSinceStopPressed >= 2000) {
          	 // RGB light as progress
        	tone(piezo, 800, 250);
          	delay(500);
          	tone(piezo, 800, 250);
          	screen = 1;
          	LCD.clear();
            LCD.setCursor(0, 0); 
            LCD.print("Returning to");
          	LCD.setCursor(0, 1);
          	LCD.print("home screen..."); 
            delay(3000); // can't have anyone pressing this too soon because they will immediately select count down
        }
      }
    }
  }
  
  // Delays 
  
  if (screen == 3) {
    delay(delayTime); // only if this is the current screen
  	// Serial.println(counter); debug
  } else {
    if ((screen == 2) and (timer % 60 == 0) and (timer != 0)) {

    	setcolor(3, 142, 138); 
      	tone(piezo, 1500, 1000);  	
      	setcolor(LOW, LOW, LOW);
    }
    delay(250); 
  	Serial.print(timer);
    Serial.print(" ");
    Serial.print(updateCycle);
    if (screen == 4) {
    	Serial.print(" ");
      	Serial.print(t);
    }
    Serial.println("");
  }
}
