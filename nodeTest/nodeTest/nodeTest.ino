#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}
//#include "MFRC522.h"
#define RST_PIN 4 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 rfid(SS_PIN, RST_PIN);   // Create MFRC522 instance
byte nuidPICC[4];

//Green
//LiquidCrystal_I2C lcd(0x3F, 20, 4);
//Blue
LiquidCrystal_I2C lcd(0x27, 20, 4);
//07460985636
//Create a different twitter feed and give credentials to Paul
const char* ssid = "AndroidWannaBe";
const char* password = "sgs8heyy";
//const char* ssid = "justin";
//const char* password = "prototype";
int button1 = 16; //D2(gpio4)
int button2 = 15;
int buttonState1=0;
int buttonState2=0;
 
void setup () {
  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  Serial.begin(9600);
  
  lcd.begin(20,4);
  lcd.init();
  
  // Turn on the backlight.
  lcd.backlight();
  wipeLines(10);

  sayHello(0);
  
  pinMode(button1, INPUT);
  
  pinMode(button2, INPUT);
  delay(5000);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
 
  }
  Serial.print("WIFI Connected");
  lcd.setCursor(16, 3);
  lcd.print("WiFi");

  SPI.begin();           // Init SPI bus
  Serial.print("SPI Initialized");
  rfid.PCD_Init();    // Init MFRC522
  Serial.print("RFID Initialized");
  //delay(2000);
  //WiFi.mode(WIFI_STA); 
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
  //Serial.print("Delay passed");
}

 //Use two buttons to select between either tutorial mode, or the mode for basic tweet&weather&sms bot
void loop() {

   readRFID();
  
   buttonState1=digitalRead(button1); // put your main code here, to run repeatedly:
   if (buttonState1 == 1)
   {
     delay(2000);
   }
   buttonState2=digitalRead(button2); 
   if (buttonState2 == 1)
   {
     delay(3000);
   }
   delay(200);
}

void readRFID(){
    // Look for new cards
    if ( ! rfid.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if ( ! rfid.PICC_ReadCardSerial()) 
    {
      return;
    }
    //Show UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < rfid.uid.size; i++) 
    {
       Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(rfid.uid.uidByte[i], HEX);
       content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(rfid.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    content = content.substring(1);
    //Serial.println("AM I a happy person?START" + content.substring(1) + "END");
    confirmAttendance(content);
}

void confirmAttendance(String NUID){
   if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
     HTTPClient http;    //Declare object of class HTTPClient
   
     http.begin("http://tweety.gq/APIRequests.php");      //Specify request destination
     http.addHeader("Content-Type", "text/plain");  //Specify content-type header

     String textSend = "Crd:" + NUID + "Node_1";
     //String textSend = "Crd:" + NUID + "Node_2";
     int httpCode = http.POST(textSend);   //Send the request
     //String payload = http.getString();                  //Get the response payload
     String toPrint = "Card read: " + http.getString();
     Serial.println(httpCode);   //Print HTTP return code
     Serial.println(toPrint);    //Print request response payload
     http.end();  //Close connection
     printLongToLCD(toPrint, 0);
     delay(3000);
     tutorialMode(0);
     loop();
   }else{
      Serial.println("Error in WiFi connection");   
   }
}

boolean printLongToLCD(String textToPrint, int delayTime){
  Serial.println(textToPrint);
  int textLength = textToPrint.length();
  delay(delayTime);
  wipeLines(0);
  if(textLength > 140){
    textDidNotFit();
    return false;
  }else{
    int idx = 0;
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 20; j++){
        lcd.setCursor(j,i);
        lcd.print(textToPrint[idx]);
        idx++;
        if(idx == textLength){
          return true;
        }
      }
    }
    if(textLength > 80){
      textToPrint = textToPrint.substring(80,textLength);
      printLongToLCD(textToPrint,10000);
    }
    return true;
  }
}

void textDidNotFit(){
  wipeLines(10);
  lcd.setCursor(4, 0);
  lcd.print("Tweet is too long");
}

void sayHello(int delayTime){
  delay(delayTime);
  wipeLines(0);

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(4, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("Hello Justin!");
  lcd.setCursor(7, 1);
  lcd.print(" O O ");
  lcd.setCursor(7, 2);
  lcd.print("_____");
  lcd.setCursor(7, 3);
  lcd.print("    U");
}

void tutorialMode(int delayTime){
  delay(delayTime);
  wipeLines(0);

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(0, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("Check yourself!");
  lcd.setCursor(0,2);
  lcd.print("Tap your student");
  lcd.setCursor(0, 3);
  lcd.print("card on me.");
}

void wipeLines(int delayTime) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 20; x++) {
      lcd.setCursor (x,y);
      lcd.print(" ");
      delay(delayTime);
    }
  }
}
