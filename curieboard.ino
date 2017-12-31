
#include <CurieBLE.h>
#include "CurieIMU.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C LCD 객체 선언

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = 13; // pin to use for the LED
boolean stepEventsEnabeled = true;   // whether you're polling or using events
long lastStepCount = 0;              // step count on previous polling check
boolean blinkState = false;          // state of the LED
int measurePin = 1; //Connect dust sensor to Arduino A1 pin
int ledPower = 4;   //Connect 3 led driver pins of dust sensor to Arduino D4
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
int rst=0;
String firstline;
String secondline;
int airstate=-1;
int speakerpin = 12; //스피커가 연결된 디지털핀 설정

void setup() {
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  //count on
  while(!Serial) ;    // wait for serial port to connect.
  Serial.println("Serial Opened");
   // begin initialization
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setLocalName("MISAE.AI");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  CurieIMU.begin();
  // turn on step detection mode:
  CurieIMU.setStepDetectionMode(CURIE_IMU_STEP_MODE_NORMAL);
  // enable step counting:
  CurieIMU.setStepCountEnabled(true);
  
  if (stepEventsEnabeled) {
    // attach the eventCallback function as the
    // step event handler:
    //CurieIMU.attachInterrupt(eventCallback);
    //CurieIMU.interrupts(CURIE_IMU_STEP);  // turn on step detection

    Serial.println("IMU initialisation complete, waiting for events...");
  }
  
  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  lcd.init(); // lcd를 사용을 시작합니다.
  lcd.backlight();
  lcd.print("Welcome to JHIJ!"); // lcd 화면에 만보기 출력
  lcd.setCursor(0,1); 
  lcd.print("<Move Body Now!>"); // 둘째줄 출력
  for(int i=0;i<2;i++)
  {
    delay(1000);
    lcd.scrollDisplayRight();
    delay(1000);
    lcd.scrollDisplayLeft();
    delay(1000);
    lcd.scrollDisplayLeft();
    delay(1000);
    lcd.scrollDisplayRight();
  }
}

void loop() {

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
       /*
        //count whenever
       if (!stepEventsEnabeled) {
         updateStepCount();
       }
       */
       digitalWrite(13, blinkState);
       blinkState = !blinkState;
       delay(1000);
 
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          Serial.println("LED on");
          digitalWrite(ledPin, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(ledPin, LOW);          // will turn the LED off
        }
      }
      updateStepCount();
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
  updateStepCount();
}

static void updateStepCount() 
{
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
   voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
  rst=(int(dustDensity*1000/2.5));
  if(rst<0)
  {
   rst=0;
  }
  // get the step count:
  int stepCount = CurieIMU.getStepCount();

  // if the step count has changed, print it:
  if (stepCount != lastStepCount) {
    Serial.print("Step count: ");
    Serial.println(stepCount);
    switchCharacteristic.setValue(stepCount);
    
    // save the current count for comparison next check:
    lastStepCount = stepCount;
  }
  float nearest = roundf(float(stepCount)/272.8 * 100) / 100; 
  if (rst <= 30) {   //만약 미세먼지 값이 0.01 보다 크고 30이랑 같거나 작으면 아래를 출력
   firstline="AirGood ";
   airchk(0);
  }
  else if (rst > 30 && rst <= 80) {  //만약 미세먼지 값이 30보다 크고 80이랑 같거나 작으면 아래를 출력
    firstline="AirSoSo ";
    airchk(1);
  }
  else if (rst > 80 && rst <= 150) {  //만약 미세먼지 값이 80보다 크고 150이랑 같거나 작으면 아래를 출력
    firstline="AirBAD! ";
    airchk(2);
  }
  else if (rst > 150) {  //만약 미세먼지 값이 150 보다 크면 아래를 출력
    firstline="AirHELL ";
    airchk(3);
  }
  firstline+=(String(rst)+"ug/m3");
  secondline="-"+String(nearest)+"g "+String(stepCount)+"walks";
  lcd.clear();    
  lcd.print(firstline); // lcd 화면에 만보기 출력
  lcd.setCursor(0,1); 
  lcd.print(secondline); // lcd 화면에 미세먼지 출력
  delay(5000);
}
void airchk(int curair){
  if(airstate<curair){
      for(int i=0;i<3;i++)
      {
        tone(speakerpin,500,1000);  //500: 음의 높낮이(주파수), 1000: 음의 지속시간(1초)
        lcd.noBacklight();
        delay(1000);
        lcd.backlight();
        delay(1000);
      }
  }
  airstate=curair;
}
static void eventCallback(void) {
  if (CurieIMU.stepsDetected())
    updateStepCount();
}