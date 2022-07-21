/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

int upButton = 12;
int downButton = 13;
int selectButton = 14;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int Data;
  float x;
  float y;
} struct_message;


// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;

// Create an array with all the structures
struct_message boardsStruct[1] = {board1};


// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
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


  delay(50);
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Level : ");
  lcd.setCursor(8, 0);
  lcd.println(boardsStruct[myData.id - 1].y, 2);
  lcd.setCursor(13, 0);
  lcd.print("%  ");
  //   lcd.printf("Level : %f \n", boardsStruct[myData.id - 1].y);
  //  lcd.printf("Level : %f \n", roundf(boardsStruct[myData.id - 1].y *100.0)/100.0);
  //  delay(1000);
}



void setup() {
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Initialize Serial Monitor
  Serial.begin(115200);

  //Init pin

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  
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
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  // Access the variables for each board
  /*int board1X = boardsStruct[0].x;
    int board1Y = boardsStruct[0].y;
    int board2X = boardsStruct[1].x;
    int board2Y = boardsStruct[1].y;
  */
}
