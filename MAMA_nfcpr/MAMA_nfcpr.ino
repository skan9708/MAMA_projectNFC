/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example to change UID of changeable MIFARE card.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * This sample shows how to set the UID on a UID changeable MIFARE card.
 * NOTE: for more informations read the README.rst
 * 
 * @author Tom Clement
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSPEMPTY   50        D12        ICSPEMPTY           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Hack.h>
#define EMPTY 255

// 1: 1번 저장소 이용, 2: 2번 저장소 .... 6: write mode toggle (default -readmode) 7: setting init
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522Hack mfrc522Hack(&mfrc522);  // Create MFRC522Hack instance.


/* Set your new UID here! */
byte newUid[5][4] = {{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00}};

byte selectNum = EMPTY;
bool powerStatus = true;
bool writeMode = false;
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  
  mfrc522.PCD_Init();  // Init MFRC522 card
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
  Serial.println(F("please touch the card that you are going to copy"));
  
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

// But of course this is a more proper approach
void readUID(){
  
  for (byte i = 0; i < mfrc522.uid.size; ++i) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    
    Serial.print(mfrc522.uid.uidByte[i],HEX);

    newUid[selectNum][i] = mfrc522.uid.uidByte[i];
  } 
  selectNum= EMPTY;
}
void writeUID(){
  if ( mfrc522Hack.MIFARE_SetUid(newUid[selectNum], (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
  }
  Serial.println("please touch the card that would be copied");
  selectNum=EMPTY;
}
void printUIDS(){
  for(byte j = 0; j < 5; ++j){ 
      Serial.print(j+1);
      Serial.print("번째 저장소 : ");
          for (byte i = 0; i < mfrc522.uid.size; ++i) {
            Serial.print(newUid[j][i]);
            Serial.print(" ");
          }
          Serial.println(); 
        }
      
}
void loop() {
  if(selectNum == EMPTY){
     Serial.print(F("값을 입력해 주세요:\n"));
  }
  else{
     byte temp = Serial.read()-'0';
     if(temp == 7){
      
        selectNum=EMPTY; 
        printUIDS();
     }
      
    Serial.println(selectNum);
    Serial.print("writeMode:");
    Serial.println(writeMode);
  }
  if(Serial.available()){
    if(selectNum == EMPTY){
      
      selectNum = Serial.read()-'0';
    }
    if(selectNum == 7){
      printUIDS();
     selectNum = EMPTY; 
     return;
    }
    if(selectNum == 6){
      Serial.println("sibal writeMode");
      writeMode = !writeMode;
      selectNum = EMPTY;
      return;
    }
    else{
      --selectNum;
    }
  }
  
    // Look for new cards, and select one if present
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      delay(50);
      return;
    }
    
    // Now a card is selected. The UID and SAK is in mfrc522.uid.
    
    // readmode
    Serial.print(F("Card UID:"));
    if(selectNum >= 0 && selectNum<5){
      if(writeMode){
        writeUID();
      }
      else{
        readUID();
      }
    }
 
    delay(5000);
    // Set new UID
    // Halt PICC and re-select it so DumpToSerial doesn't get confused
    mfrc522.PICC_HaltA();
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      return;
    }
    
    // Dump the new memory contents
    Serial.println(F("New UID and contents:"));
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  delay(2000);
}
