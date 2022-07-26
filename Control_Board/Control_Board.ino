/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>

// REPLACE WITH RECEIVER MAC Address
//Send to Sensor Board
uint8_t broadcastAddress[] = {0x84, 0xF3, 0xEB, 0xCC, 0xBB, 0x52};


int upButton = 12;
int downButton = 13;
int selectButton = 14;
int tankHeight = 300;
int addr = 0;
String pumpStatus = "A";
#define EEPROM_SIZE 512


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
float level = 10.0;

const long interval = 10000;
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int Data;
  float x;
  float y;
  int setPoint;
} struct_message;


// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Create a struct_message called DHTReadings to hold sensor readings
struct_message mySentData;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;

// Create an array with all the structures
struct_message boardsStruct[1] = {board1};

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }

}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {

  //   lcd.printf("Level : %f \n", boardsStruct[myData.id - 1].y);
  //  lcd.printf("Level : %f \n", roundf(boardsStruct[myData.id - 1].y *100.0)/100.0);
  //  delay(1000);



  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    //Set values to send
    myData.setPoint = tankHeight;

    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    //    lastTime = millis();

  }
  lcd.clear();
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id - 1].Data = myData.Data;
  boardsStruct[myData.id - 1].x = myData.x;
  boardsStruct[myData.id - 1].y = myData.y;
  Serial.printf("Data Value: %d \n", boardsStruct[myData.id - 1].Data);
  Serial.printf("x value: %f \n", boardsStruct[myData.id - 1].x);
  Serial.printf("y value: %f \n", boardsStruct[myData.id - 1].y);
  Serial.println();
  level = boardsStruct[myData.id - 1].y;

  delay(50);
  //   set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Level : ");
  lcd.setCursor(8, 0);
  lcd.println(level, 2);
  lcd.setCursor(13, 0);
  lcd.print("%  ");
  lcd.setCursor(0, 1);
  lcd.printf("Size: %dCM", tankHeight);
  lcd.setCursor(12, 1);
  lcd.print(pumpStatus);

}



void setup() {
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Initialize Serial Monitor
  Serial.begin(115200);

  //Init EEPROM
  EEPROM.begin(EEPROM_SIZE);
  //Init pin

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);

  //  tankHeight = int(EEPROM.read(addr));
  //  tankHeight = int(EEPROM.read(addr));
  //  tankHeight = int(EEPROM.read(addr));
  //  EEPROM.commit();

  updateMenu();
  if (!tankHeight) {
    tankHeight = tankHeight;
  }

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  // Set ESP-NOW Role


  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);



}

void loop() {
  //
  //  if (!digitalRead(downButton)) {
  //    if (tankHeight > 0) {
  //      tankHeight--;
  //      updateMenu();
  //      delay(100);
  //      while (!digitalRead(upButton));
  //    }
  //  }
  //  if (!digitalRead(upButton)) {
  //    if (tankHeight < 400) {
  //      tankHeight++;
  //      updateMenu();
  //      delay(100);
  //      while (!digitalRead(downButton));
  //    }
  //  }
  //  if (!digitalRead(selectButton)) {
  //    executeAction();
  //    updateMenu();
  //    delay(500);
  //    while (!digitalRead(selectButton));
  //  }



}



void updateMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Level : ");
  lcd.setCursor(8, 0);
  lcd.println(level, 2);
  lcd.setCursor(13, 0);
  lcd.print("%  ");
  lcd.setCursor(0, 1);
  lcd.printf("Size: %dCM", tankHeight);
  lcd.setCursor(12, 1);
  lcd.print(pumpStatus);


}

void executeAction() {
  int compareValue = int(EEPROM.read(addr));
  if (compareValue == tankHeight) {
    if (pumpStatus == "M") {
      pumpStatus = "A";
    }
    else if (pumpStatus == "A") {
      pumpStatus = "M";
    }
  } else {
    EEPROM.write(addr, tankHeight);
    EEPROM.commit();
    lcd.clear();
    lcd.print(">Storing.......");
    delay(1500);
  }


}
