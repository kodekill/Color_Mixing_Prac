#include <Adafruit_NeoPixel.h>
#define PIN 2
#define NUM_OF_LEDS 1
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
  /
//Max pot values for 3.3v will be 670
//Max pot values for 5.0v will be 1024
int Level = A0; 
int Tone = A1;

int LevelValue = 0;
int ToneValue = 0;
unsigned int rate = 5;
//I'm using this value for now just to keep the light somewhat dim while I proto this
float myIntensity = .02;  //2% brightness 

enum AnimationMode{
  ON,
  BREATHE,
  BLINK,
  CYCLE,
} mode = ON;

typedef struct {
  int R;
  int G;
  int B;
  int intensity;
} RGB;
  
RGB color, colorPrime;


//int DISPLAY_TIME = 10;  // In milliseconds


void setup()
{
  color = {0, 0, 0, 5};
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
}


int Clamp(int x) {
  if (x < 0) 
  return 0;
  
  else if (x > 255)
  return 255;
  
  else 
  return x;
}



void increaseIntensity() {
  color.intensity = Clamp(color.intensity + 25);
}

void decreaseIntensity() {
  color.intensity = Clamp(color.intensity - 25);
}

void loop() 
{
  Take_Color_Input();
  AnimationStep();
  delay(10);
  SetPixels();
}


int AnimationStep(){
  LevelValue = analogRead(Level);  //reads from Level Pot

  if ((LevelValue >= 0 )    && (LevelValue <= 225))   { mode  = ON;}
  if ((LevelValue >= 226 )   && (LevelValue <= 450))  { mode  = BREATHE;}
  if ((LevelValue >= 451 )  && (LevelValue <= 676))  { mode  = BLINK;}

  //Serial.print("Level = ");
  //Serial.println(LevelValue);

  switch (mode){
    
     case ON:
        Full_On();
        //Serial.println("AnimationStep = ON");
     break;

     case BREATHE:
         Breathe();
         //Serial.println("AnimationStep = BREATHE");
     break;

     case BLINK:
         Blink();
         //Serial.println("AnimationStep = BLINK");
     break;
     
     case CYCLE:
          Cycle();
          //Serial.println("AnimationStep = CYCLE");
  }
}


void Take_Color_Input()
{
  ToneValue = analogRead(Tone);    // reads from Tone pot

if (ToneValue <= 255)          // Red to Yellow
  {
    color.R = 255 - ToneValue;           // red goes from on to off
    color.G = ToneValue;                 // green goes from off to on
    color.B = 0;                         // blue is always off
  }

else if (ToneValue <= 511)     // Green to Blue
  {
    color.R = 0;                         // red is always off
    color.G = 255 - (ToneValue - 256);   // green on to off
    color.B = (ToneValue - 256);         // blue off to on
  }

else // color >= 512       // Purple to Red
  {
    color.R = (ToneValue - 512);          // red off to on
    color.G = 0;                          // green is always off
    color.B = 255 - (ToneValue - 512);    // blue on to off
  }
}

void Cycle(){
int x; 

  for (x=0; x < 768; x++){
    delay(10);
    //Serial.println(x);
    if (x <= 255)          // Red to Yellow
      {
        color.R = 255 - x;              // red goes from on to off
        color.G = x;                    // green goes from off to on
        color.B = 0;                    // blue is always off
      }
    
    else if (x <= 511)     // Green to Blue
      {
        color.R = 0;                    // red is always off
        color.G = 255 - (x - 256);      // green on to off
        color.B = (x - 256);            // blue off to on
      }
    
    else // color >= 512       // Purple to Red
      {
        color.R = (x - 512);             // red off to on
        color.G = 0;                     // green is always off
        color.B = 255 - (x - 512);       // blue on to off
      }
      colorPrime = color;
  colorPrime.intensity = 3;
  } 
}

void Full_On(){
  colorPrime = color;
  colorPrime.intensity = 3;
}

void Breathe(){
static enum {
   inhale,
   exhale 
  }respiration = exhale;

  static int brightness = 400;

 if (brightness > color.intensity){
      brightness = color.intensity;
      respiration = exhale;
 }
  
  if (respiration == exhale){
    if (brightness == 0){
      respiration = inhale;
      brightness++;
    }
    else 
      brightness--;
  }

  else{
    if (brightness == color.intensity){
      respiration = exhale;
      brightness--;
    }
    else brightness++;
  }

  // use the value of colorPrime for output 
  colorPrime = color;
  colorPrime.intensity = brightness;
  delay(12*rate);
}

void Blink(){
  static enum{
    on,
    off,
    middle
  }type = on;

  static int brightness = 6;

  if (type == on){
    delay(100*rate);
    brightness = 8;
    type = middle;
  }
  
  else if (type == middle){
    type = off;
  }
  
  else {
    delay(100*rate);
    brightness = 0;
    type = on;
  }
  
  colorPrime = color;
  colorPrime.intensity = brightness;
}

void SetPixels(){
  Serial.println(colorPrime.intensity);
  SetLED(strip.Color(colorPrime.R * colorPrime.intensity*myIntensity, colorPrime.G * colorPrime.intensity*myIntensity, colorPrime.B * colorPrime.intensity*myIntensity), 0); 
}

void SetLED(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}




