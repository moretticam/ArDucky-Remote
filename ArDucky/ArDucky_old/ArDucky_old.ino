/*
 * =========================================================
 *   Copyright (c) 2019 moretticam, (Licensed under MIT)
 *  For more information see: github.com/moretticam/arducky
 * =========================================================
 * 
 */
#include <VirtualWire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SD.h>

#include "Configuration.h"
// #include "ConfigurationProMicro.h" // Select this if you have a Sparkfun ProMicro board
#include "Keyboard.h"

 MFRC522 mfrc522(RFID_SDA, RFID_RST); //asoidjasopidjasdj ehlo

const int KEYPAD_0 = 234;
const int KEYPAD_1 = 225;
const int KEYPAD_2 = 226;
const int KEYPAD_3 = 227;
const int KEYPAD_4 = 228;
const int KEYPAD_5 = 229;
const int KEYPAD_6 = 230;
const int KEYPAD_7 = 231;
const int KEYPAD_8 = 232;
const int KEYPAD_9 = 233;
const int KEYPAD_ASTERIX = 221;
const int KEYPAD_ENTER = 224;
const int KEYPAD_MINUS = 222;
const int KEYPAD_PERIOD = 235;
const int KEYPAD_PLUS = 223;
const int KEYPAD_SLASH = 220;
const int PRINTSCREEN = 206;

const int buffersize = 256;

char* buf = malloc(sizeof(char) * buffersize);
char* repeatBuffer = malloc(sizeof(char) * 12);

int bufSize = 0;
int defaultDelay = 10;
int defaultCharDelay = 10;
int rMin = -100;
int rMax = 100;
bool isSD = false;

int getSpace(int start, int end) {
  for (int i = start; i < end; i++) {
    if (buf[i] == ' ') return i;
  }
  return -1;
}

bool equals(char* strA, int start, int end, char* strB, int strLen) {
  if (end - start != strLen) return false;
  for (int i = 0; i < strLen; i++) {
    if (strA[start + i] != strB[i]) return false;
  }
  return true;
}

int toPositive(int num) {
  if (num < 0) return num * (-1);
  else return num;
}

bool equalsBuffer(int start, int end, char* str) {
  return equals(buf, start, end, str, String(str).length());
}

int getInt(char* str, int pos) {
  if (equals(str, pos + 1, pos + 7, "RANDOM", 6)) {
    return random(rMin, rMax);
  } else {
    String a = "";
    for (int i = pos + 1; i < pos + 6; i++) {
      a += String(buf[i]);
    }
    return a.toInt();
  }
}

void KeyboardWrite(uint8_t c) {
  Keyboard.press(c);
  delay(defaultCharDelay);
  Keyboard.release(c);
}

void runLine() {
  if (DEBUG) {
    Serial.println("run: '" + String(buf).substring(0, bufSize) + "' (" + (String)bufSize + ")");
  }

  int space = getSpace(0, bufSize);

  if (space == -1) runCommand(0, bufSize);
  else {
    if (equalsBuffer(0, space, "DEFAULTDELAY") || equalsBuffer(0, space, "DEFAULT_DELAY")) defaultDelay = getInt(buf, space);
    else if (equalsBuffer(0, space, "DEFAULTCHARDELAY") || equalsBuffer(0, space, "DEFAULT_CHAR_DELAY")) defaultCharDelay = getInt(buf, space);
    else if (equalsBuffer(0, space, "DELAY")) delay(getInt(buf, space));
    else if (equalsBuffer(0, space, "STRING")) {
      for (int i = space + 1; i < bufSize; i++) KeyboardWrite(buf[i]);
    }
    
    else {
      runCommand(0, space);
      while (space >= 0 && space < bufSize) {
        int nSpace = getSpace(space + 1, bufSize);
        if (nSpace == -1) nSpace = bufSize;
        runCommand(space + 1, nSpace);
        space = nSpace;
      }
    }
  }

  Keyboard.releaseAll();
  delay(defaultDelay);
}

void runCommand(int s, int e) {
  if (DEBUG) {
    Serial.println("Press '" + String(buf).substring(s, e) + "'");
  }

  if (e - s < 2) Keyboard.press(buf[s]);
  else if (equalsBuffer(s, e, "ENTER")) Keyboard.press(KEY_RETURN);
  else if (equalsBuffer(s, e, "GUI") || equalsBuffer(s, e, "WINDOWS")) Keyboard.press(KEY_LEFT_GUI);
  else if (equalsBuffer(s, e, "SHIFT")) Keyboard.press(KEY_LEFT_SHIFT);
  else if (equalsBuffer(s, e, "ALT")  || equalsBuffer(s, e, "ALT_LEFT") || equalsBuffer(s, e, "ALTLEFT")) Keyboard.press(KEY_LEFT_ALT);
  else if (equalsBuffer(s, e, "ALT_RIGHT") || equalsBuffer(s, e, "ALTRIGHT")) Keyboard.press(KEY_RIGHT_ALT);
  else if (equalsBuffer(s, e, "CTRL") || equalsBuffer(s, e, "CONTROL")) Keyboard.press(KEY_LEFT_CTRL);
  else if (equalsBuffer(s, e, "CAPSLOCK")) Keyboard.press(KEY_CAPS_LOCK);
  else if (equalsBuffer(s, e, "DELETE")) Keyboard.press(KEY_DELETE);
  else if (equalsBuffer(s, e, "END")) Keyboard.press(KEY_END);
  else if (equalsBuffer(s, e, "ESC") || equalsBuffer(s, e, "ESCAPE")) Keyboard.press(KEY_ESC);
  else if (equalsBuffer(s, e, "HOME")) Keyboard.press(KEY_HOME);
  else if (equalsBuffer(s, e, "INSERT")) Keyboard.press(KEY_INSERT);
  else if (equalsBuffer(s, e, "PAGEUP")) Keyboard.press(KEY_PAGE_UP);
  else if (equalsBuffer(s, e, "PAGEDOWN")) Keyboard.press(KEY_PAGE_DOWN);
  else if (equalsBuffer(s, e, "SPACE")) Keyboard.press(' ');
  else if (equalsBuffer(s, e, "TAB")) Keyboard.press(KEY_TAB);
  else if (equalsBuffer(s, e, "BACKSPACE")) Keyboard.press(KEY_BACKSPACE);

  else if (equalsBuffer(s, e, "UP") || equalsBuffer(s, e, "UPARROW")) Keyboard.press(KEY_UP_ARROW);
  else if (equalsBuffer(s, e, "DOWN") || equalsBuffer(s, e, "DOWNARROW")) Keyboard.press(KEY_DOWN_ARROW);
  else if (equalsBuffer(s, e, "LEFT") || equalsBuffer(s, e, "LEFTARROW")) Keyboard.press(KEY_LEFT_ARROW);
  else if (equalsBuffer(s, e, "RIGHT") || equalsBuffer(s, e, "RIGHTARROW")) Keyboard.press(KEY_RIGHT_ARROW);

  else if (equalsBuffer(s, e, "PRINTSCREEN")) Keyboard.press(PRINTSCREEN);

  else if (equalsBuffer(s, e, "F1")) Keyboard.press(KEY_F1);
  else if (equalsBuffer(s, e, "F2")) Keyboard.press(KEY_F2);
  else if (equalsBuffer(s, e, "F3")) Keyboard.press(KEY_F3);
  else if (equalsBuffer(s, e, "F4")) Keyboard.press(KEY_F4);
  else if (equalsBuffer(s, e, "F5")) Keyboard.press(KEY_F5);
  else if (equalsBuffer(s, e, "F6")) Keyboard.press(KEY_F6);
  else if (equalsBuffer(s, e, "F7")) Keyboard.press(KEY_F7);
  else if (equalsBuffer(s, e, "F8")) Keyboard.press(KEY_F8);
  else if (equalsBuffer(s, e, "F9")) Keyboard.press(KEY_F9);
  else if (equalsBuffer(s, e, "F10")) Keyboard.press(KEY_F10);
  else if (equalsBuffer(s, e, "F11")) Keyboard.press(KEY_F11);
  else if (equalsBuffer(s, e, "F12")) Keyboard.press(KEY_F12);

  else if (equalsBuffer(s, e, "NUM_0")) KeyboardWrite(KEYPAD_0);
  else if (equalsBuffer(s, e, "NUM_1")) KeyboardWrite(KEYPAD_1);
  else if (equalsBuffer(s, e, "NUM_2")) KeyboardWrite(KEYPAD_2);
  else if (equalsBuffer(s, e, "NUM_3")) KeyboardWrite(KEYPAD_3);
  else if (equalsBuffer(s, e, "NUM_4")) KeyboardWrite(KEYPAD_4);
  else if (equalsBuffer(s, e, "NUM_5")) KeyboardWrite(KEYPAD_5);
  else if (equalsBuffer(s, e, "NUM_6")) KeyboardWrite(KEYPAD_6);
  else if (equalsBuffer(s, e, "NUM_7")) KeyboardWrite(KEYPAD_7);
  else if (equalsBuffer(s, e, "NUM_8")) KeyboardWrite(KEYPAD_8);
  else if (equalsBuffer(s, e, "NUM_9")) KeyboardWrite(KEYPAD_9);
  else if (equalsBuffer(s, e, "NUM_ASTERIX")) KeyboardWrite(KEYPAD_ASTERIX);
  else if (equalsBuffer(s, e, "NUM_ENTER")) KeyboardWrite(KEYPAD_ENTER);
  else if (equalsBuffer(s, e, "NUM_Minus")) KeyboardWrite(KEYPAD_MINUS);
  else if (equalsBuffer(s, e, "NUM_PERIOD")) KeyboardWrite(KEYPAD_PERIOD);
  else if (equalsBuffer(s, e, "NUM_PLUS")) KeyboardWrite(KEYPAD_PLUS);

  else if (DEBUG) {
    Serial.println("failed to find command");
  }
  /* not implemented
    else if(equalsBuffer(s,e,"APP")) Keyboard.press();
    else if(equalsBuffer(s,e,"MENU")) Keyboard.press();
    else if(equalsBuffer(s,e,"BREAK") || equalsBuffer(s,e,"PAUSE",5)) Keyboard.press();
    else if(equalsBuffer(s,e,"NUMLOCK")) Keyboard.press();
    else if(equalsBuffer(s,e,"SCROLLLOCK")) Keyboard.press();
  */
}

String getScriptFilename() {
  String scriptName = "";
   switch(REMOTE){
    
    case 1:{// RF
      bool RFState = false;
      while(RFState == false){
        uint8_t RFbuff[VW_MAX_MESSAGE_LEN]={""};   //clean VirtualWire receiver buffer
        uint8_t RFbuflen = VW_MAX_MESSAGE_LEN;
        delay(400);
        RFState = vw_get_message(RFbuff, &RFbuflen);
        String RFSTRBUFF = RFbuff;
        scriptName = RFSTRBUFF; 
        delay(1200);
      }
    }
    break;
    
    case 2:{// RFID
      byte Key_Value[MFRC522::MF_KEY_SIZE] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      MFRC522::MIFARE_Key key;

      Serial.print("Using key:");
      for (int i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
            key.keyByte[i] = Key_Value[i];
            Serial.print(key.keyByte[i] < 0x10 ? " 0" : " ");
            Serial.print(key.keyByte[i], HEX);
        }
      Serial.println(); 
  
      byte buffer[18];
      byte block = 1;
      MFRC522::StatusCode status;
      mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
      byte byteCount = sizeof(buffer);
      status = mfrc522.MIFARE_Read(1, buffer, &byteCount);
      Serial.print(F("Block ")); Serial.print(block); Serial.print(F(":"));
      for (byte i = 0; i < 16; i++) {
           Serial.print(buffer[i] < 0x10 ? " 0" : " ");
           Serial.print(buffer[i], HEX);
      }
      Serial.println();
      char RFIDSTRBUFF[4];
      for(byte a = 0; a < 4; a++){
        RFIDSTRBUFF[a] = char(buffer[a]);
        char biit = RFIDSTRBUFF[a];
        scriptName += biit;
        }
      
      mfrc522.PCD_StopCrypto1();
      mfrc522.PICC_HaltA();
      
      
      }
    break; 
    
    default: // no remote
      if (N_DIP >= 1) {
        if (digitalRead(DIP_1) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      } else {
        scriptName = SCRIPT_NAME;
      }
        
      if (N_DIP >= 2) {
        if (digitalRead(DIP_2) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      } 

      if (N_DIP >= 3) {
        if (digitalRead(DIP_3) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }
      
      if (N_DIP >= 4) {
        if (digitalRead(DIP_4) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }

      if (N_DIP >= 5) {
        if (digitalRead(DIP_5) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }

      if (N_DIP >= 6) {
        if (digitalRead(DIP_6) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }

      if (N_DIP >= 7) {
        if (digitalRead(DIP_7) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }

      if (N_DIP >= 8) {
        if (digitalRead(DIP_8) == LOW) {
          scriptName += '1';
        } else {
          scriptName += '0';
        }
      }
    }
  scriptName += ".txt";
  return scriptName;
}






void executePayload() {
  File payload;
  File logfile;
  digitalWrite(LED, HIGH); 
  String payload_name = "0001.txt";
  Serial.println(payload_name);
  payload = SD.open(payload_name);
  delay(50);
  Serial.println(payload);
  logfile = SD.open(LOG_NAME, FILE_WRITE);
  
  if (!payload) {
      
    if (DEBUG) {
      Serial.println("couldn't find script: '" + String(payload_name) + "'");
    }
    if (LOG) {
      logfile.print("couldn't find script");
      logfile.print("\n");
    }

    
  } else {
    if (LOG) {
      logfile.print("Opened file!");
      logfile.print("\n");
    }
    Keyboard.begin();
    while (payload.available()) {

      buf[bufSize] = payload.read();
      if (buf[bufSize] == '\r' || buf[bufSize] == '\n' || bufSize >= buffersize) {
        if (buf[bufSize] == '\r' && payload.peek() == '\n') payload.read();

        //---------REPEAT---------
        int repeatBufferSize = 0;
        int repeats = 0;
        unsigned long payloadPosition = payload.position();

        for (int i = 0; i < 12; i++) {
          if (payload.available()) {
            repeatBuffer[repeatBufferSize] = payload.read();
            repeatBufferSize++;
          } else break;
        }

        if (repeatBufferSize > 6) {
          if (equals(repeatBuffer, 0, 6, "REPEAT", 6)) {
            repeats = getInt(repeatBuffer, 6);
          }
        }

        for (int i = 0; i < repeats; i++) runLine();

        payload.seek(payloadPosition);
        //------------------------

        runLine();
        bufSize = 0;
      }
      else bufSize++;
    }
    if (bufSize > 0) {
      runLine();
      bufSize = 0;
    }
    payload.close();
    if (LOG) {
      logfile.print("Done!");
      logfile.print("\n");
      logfile.close();
    }
    Keyboard.end();
  }
}

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while(!Serial);
    delay(1000);
    Serial.println("Enjoy with ArDucky!");
    delay(1000);
  }
  
  switch(REMOTE){
    case 1:{
      vw_set_rx_pin(RECEIVER_DATA);
      vw_setup(2000);
      vw_rx_start();
    }
      break; 
    case 2:{
      Serial.println("RFID");
      delay(1000);
      //mfrc522.PCD_Init();
    }
    break;
    default: 
      Serial.println("No remote selected");
      pinMode(DIP_1, INPUT_PULLUP);
      pinMode(DIP_2, INPUT_PULLUP);
      pinMode(DIP_3, INPUT_PULLUP);
      pinMode(DIP_4, INPUT_PULLUP);
      pinMode(DIP_5, INPUT_PULLUP);
      pinMode(DIP_6, INPUT_PULLUP);
      pinMode(DIP_7, INPUT_PULLUP);
      pinMode(DIP_8, INPUT_PULLUP);
      if (BUTTON_EXECUTE != 0) {
        pinMode(BUTTON_EXECUTE, INPUT_PULLUP);
      } else {
        delay(1000);
        executePayload();
      }
  }
  
  

  randomSeed(analogRead(0));

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  delay(1000);
  //executePayload();

  if (!SD.begin(SDCARD_CS)z) {
    if (DEBUG) {
        Serial.println("couldn't access sd-card :(");
    }
  } else {
    isSD = true;
  }
}

void loop() {
  digitalWrite(LED, LOW);

  switch(REMOTE){
    case 1:{
      if (isSD) {
      executePayload();
      } 
    }
      break; 
    case 2:{
      if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
      
        executePayload(); 
      
       if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
      
    }
      break; 
    default: 
      if (isSD && BUTTON_EXECUTE != 0) {
        int buttonState = digitalRead(BUTTON_EXECUTE);
        if (buttonState == LOW) {
          executePayload();
        }
      }
  }
  delay(50);// delay in between reads for stability
}
