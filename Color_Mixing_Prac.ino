#include <Adafruit_NeoPixel.h>
#define PEDAL_BUTTON 0
#define PIN 2
#define NUM_OF_LEDS 1
//Max pot values for 3.3v will be 670
#define MAX_POT 675

//Max pot values for 5.0v will be 1024
//#define MAX_POT 1024
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int Level = A0; 
int Tone = A1;
int max_pot = MAX_POT;
int num_modes = 3; //The amount of modes I have defined in my enum AnimationMode list

unsigned int LevelValue = 0;
unsigned int ToneValue = 0;
unsigned int rate = 5;

unsigned int state = 0;
unsigned int val = 0;
unsigned int old_val = 0;
//I'm using this value for now just to keep the light somewhat dim while I proto this
float myIntensity = .02;  //2% brightness 

enum AnimationMode{
  ON,
  BREATHE,
  BLINK,
  CYCLE,
  SEIZURE,
} mode = ON;

typedef struct {
  int R;
  int G;
  int B;
  int intensity;
} RGB;
  
RGB color, colorPrime;

void setup()
{
  pinMode(PEDAL_BUTTON, INPUT);
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

void loop() 
{
  Take_Color_Input();
  //Serial.println(state);
  AnimationStep();
  delay(10);
  SetPixels();
}


int AnimationStep(){
  val = digitalRead(PEDAL_BUTTON);
  LevelValue = analogRead(Level);  //reads from Level Pot

  if ((LevelValue >= 0 )                      && (LevelValue <= (max_pot/num_modes)))       { mode  = ON;}
  if ((LevelValue > (max_pot/num_modes))      && (LevelValue <= ((max_pot/num_modes)*2)))   { mode  = BREATHE;}
  if ((LevelValue > ((max_pot/num_modes)*2))  && (LevelValue <= max_pot))                   { mode  = BLINK;}

  //Serial.print("Level = ");
  //Serial.println(LevelValue);

  if ((val == HIGH) && (old_val == LOW)){
      state = 1 - state;
      delay(10);
    }
    
    old_val = val;
    
    if (state == 0) {
       //LevelValue = NULL;
       mode = SEIZURE; 
     }
  
  switch (mode){
    
     case ON:
        Cycle();
        //Full_On();
        Serial.println("AnimationStep = ON");
     break;

     case BREATHE:
         Breathe();
         //Serial.println("AnimationStep = BREATHE");
     break;

     case BLINK:
         Blink();
         //Serial.println("AnimationStep = BLINK");
     break;   

    case SEIZURE:
         Seizure();
         //Serial.println("AnimationStep = SEIZURE");
     break;
     
     case CYCLE:
         Cycle();
         //Serial.println("AnimationStep = CYCLE");
     break;
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

void Full_On(){
  colorPrime = color;
  colorPrime.intensity = 3;
}


void Breathe(){
static enum {
   inhale,
   exhale 
  }respiration = exhale;

  static int brightness = 50;

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
  Serial.println(brightness);
  delay(20*rate);
}

void Blink(){
  static enum{
    on,
    off,
    middle
  }type = on;

  static int brightness = 0;

  if (type == on){
    delay(100*rate);
    brightness = 3;
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

void Cycle(){

  for (int x = 0; x < 768; x++){
      ToneValue = x;
      colorPrime = color;
      colorPrime.intensity = 3;
      delay(4);
  }
}

void Seizure(){
  static enum{
    red,
    green,
    blue,
    yellow,
    purple,
    aqua,
    white
  }interval = red;
 
  if (interval == red)
  {
    color.R = 255,
    color.G = color.B = 0;
    interval = green;
  }

  else if (interval == green)
  {
    color.R = color.B = 0,
    color.G = 255;
    interval = blue;
  }

  else if (interval == blue)
  {
    color.R = color.G = 0,
    color.B = 255;
    interval = yellow;
  }

    else if (interval == yellow)
  {
    color.R = color.G = 255,
    color.B = 0;
    interval = purple;
  }

    else if (interval == purple)
  {
    color.R = color.B = 255,
    color.G = 0;
    interval = aqua;
  }

    else if (interval == aqua)
  {
    color.B = color.G = 255,
    color.R = 0;
    interval = white;
  }

    else if (interval == white)
  {
    color.R = color.G = color.B = 255;
    interval = red;
  }

  delay(rate * 17);
  colorPrime = color;
  colorPrime.intensity = 2;
}

void SetPixels(){
  //Serial.println(colorPrime.intensity);
  SetLED(strip.Color(colorPrime.R * colorPrime.intensity*myIntensity, colorPrime.G * colorPrime.intensity*myIntensity, colorPrime.B * colorPrime.intensity*myIntensity), 0); 
}

void SetLED(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}




