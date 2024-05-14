
#include <SevSeg.h>
//#include <stdlib.h>
#include <DHT.h>
//#include <DHT_U.h>

// Setup for temperature Sensor
SevSeg sevseg;
#define Type DHT11
int sensePin = 9;
DHT HT(sensePin,Type);
float humidity;
float tempC;
float tempF;
float threshold = 20.0;
int read;


// pins for segments of display
  byte a = A3;
  byte b = A4;
  byte c = 4;
  byte d = 5;
  byte e = 6;
  byte f = 7;
  byte g = 8;
  byte dp = 0;
// pins for digits
  byte d4 = 10;
  byte d3 = 11;
  byte d2 = 12;
  byte d1 = 13;

// Intermediate values to show on display
  int c1;
  int c2;
  int f1;
  int f2;
  int h1;
  int h2;
  uint8_t pos1;
  uint8_t pos2;
  uint8_t degree = 0b01100011;
  uint8_t celcius = 0b00111001;
  uint8_t faranheit = 0b01110001;
  uint8_t humid = 0b01110110;
  uint8_t equal = 0b01001000;

//int passiveBuzz = A2;

// variables used for humidity Interrupt
volatile  boolean flag;
volatile int button2State;

void setup() {
// setup for 4 digit display  
  byte numDigits = 4;
  byte digitPins[] = {d1,d2,d3,d4};
  byte segmentPins[] = {a,b,c,d,e,f,g,dp};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90); 

  HT.begin(); // Starting the sensor
  
  //pinMode(passiveBuzz,OUTPUT);
  //assembly line code to set pin A2 as Output pin
  asm (
      "sbi %0, %1 \n"
      : : "I" (_SFR_IO_ADDR(DDRC)), "I" (DDC2)
  );

  Serial.begin(9600);


pinMode(9, INPUT),
//humidity interrupt
  pinMode(2,INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(2),button2Pressed,CHANGE);
//interrupt to increment temperature threshold  
  pinMode(3,INPUT);
  attachInterrupt(digitalPinToInterrupt(3),button3Pressed,RISING);
    
}

void loop() {
//digitalWrite(passiveBuzz,LOW);
//digitalWrite(A2,LOW);
//assembly line code to deactivate passive buzzer at pin A2
  asm (
  "cbi %0, %1 \n"
  : : "I" (_SFR_IO_ADDR(PORTC)), "I" (PORTC2)
);

  humidity = HT.readHumidity();
  h2 = (int)humidity % 10;
  h1 = ((int)humidity - h2)/10;
  tempC = HT.readTemperature();
  c2 = (int)tempC % 10;
  c1 = ((int)tempC - c2)/10;
  tempF = (tempC * 1.8) + 32;
  f2 = (int)tempF % 10;
  f1 = ((int)tempF - c2)/10;

if (tempC>=threshold) {
 //digitalWrite(passiveBuzz,HIGH);
 //digitalWrite(A2,HIGH);
 //assembly line code to activate passive buzzer at pin A2
  asm (
  "sbi %0, %1 \n"
  : : "I" (_SFR_IO_ADDR(PORTC)), "I" (PORTC2)
  );
}

if(flag==false){ //showing temperature when right button uunpressed
  pos1 = displayVal(c1);
  pos2 = displayVal(c2);

  uint8_t cels[4] = {pos1, pos2, degree, celcius};
  sevseg.setSegments(cels);
  sevseg.refreshDisplay();
  delayDisplay();

  
  pos1 = displayVal(f1);
  pos2 = displayVal(f2);

  uint8_t far[4] = {pos1, pos2, degree, faranheit};
  sevseg.setSegments(far);
  sevseg.refreshDisplay();
  delayDisplay();
  }else{ // showing humidity when right button pressed
  
  pos1 = displayVal(h1);
  pos2 = displayVal(h2);

  uint8_t hum[4] = {humid, equal, pos1,pos2};
  sevseg.setSegments(hum);
  sevseg.refreshDisplay();
  delayDisplay();
  }
  // to Check current temperature threshold value using serial monitor 
  //Serial.print("tempeature threshold in °C: ");
  //Serial.println(threshold);

  //read= Serial.read(9);
  Serial.print(HT.read());
}

//function to convert single digit integers into a form used by the sevseg library 
uint8_t displayVal(int num){
switch (num) {
  case 0:
    return 0b00111111;
    break;
  case 1:
    return 0b00000110;
    break;
  case 2:
    return 0b01011011;
    break;
  case 3:
    return 0b01001111;
    break;
  case 4:
    return 0b01100110;
    break;
  case 5:
    return 0b01101101;
    break;
  case 6:
    return 0b01111101;
    break;
  case 7:
    return 0b01111101;
    break;
  case 8:
    return 0b01111111;
    break;
  case 9:
    return 0b01101111;
    break;  
  default:
    // if nothing else matches, do the default
    // default is optional
    return 0b00111111;
    break;
}
}

//function to refresh and delay the display
void delayDisplay(){
  for (int i = 0; i <= 1000; i++){
    //delay(1);
    // assembly line code to delay for almost 1ms
    asm volatile ( 
    "delay_loop: \n\t"
    "ldi r17, 255 \n\t"  // Load immediate value 100 into register r18
    "l1: ldi r18, 21 \n\t"  // Load immediate value 100 into register r17     
    "l2: dec r18 \n\t"  // Decrement register r18 
    "brne l2 \n\t"  // Branch if not equal to zero to delay_loop 
    "dec r17 \n\t"  // Decrement register r17 
    "brne l1 \n\t"  // Branch if not equal to zero to delay_loop
    );

    sevseg.refreshDisplay();
  }
}

//function for humidity interrupt
  void button2Pressed(){
    button2State = digitalRead(2);
    if(button2State==LOW){
       flag=true; 
    }
    if(button2State==HIGH){
       flag=false; 
    }
  }

//function for interrupt to increment threshold value
  void button3Pressed(){
       threshold++; 
  }
  
