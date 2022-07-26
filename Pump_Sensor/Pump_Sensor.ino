#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
//Send to Controller Board
uint8_t broadcastAddress[] = {0x84, 0xF3, 0xEB, 0x1E, 0x6D, 0x83};

const int RightTrigPin = 2;
const int RightEchoPin = 13;

const int LeftTrigPin = 12;
const int LeftEchoPin = 14;



// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 1

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define pauseTime 3000


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  int Data;
  float x;
  float y;
  int setPoint;
} struct_message;


// Create a struct_message called test to store variables to be sent
struct_message myData;
// Create a structure to hold the readings from each board
struct_message board1;

// Create an array with all the structures
struct_message boardsStruct[1] = {board1};

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;


long rightDuration;
long leftDuration;
float rightDistanceCm;
float rightDistancePercentage;
float leftDistanceCm;
float leftDistancePercentage;

int storageCapacity = 0; //Unit is CM


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
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
  boardsStruct[myData.id - 1].setPoint = myData.setPoint;
  storageCapacity = boardsStruct[myData.id - 1].setPoint;
  Serial.printf("Level: %d \n", boardsStruct[myData.id - 1].setPoint);
  Serial.printf("Data Value: %d \n", boardsStruct[myData.id - 1].Data);
  Serial.printf("x value: %f \n", boardsStruct[myData.id - 1].x);
  Serial.printf("y value: %f \n", boardsStruct[myData.id - 1].y);


//  delay(2000);
  // set cursor to first column, first row
}

void setup() {
  Serial.begin(115200);
  pinMode(RightTrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(RightEchoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LeftTrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(LeftEchoPin, INPUT); // Sets the echoPin as an Input


  // Set device as a Wi - Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);


}

void loop() {
  checkRightSensor();
  //  delay(pauseTime);
  checkLeftSensor();
}

void checkRightSensor() {

  //  if ((millis() - lastTime) > timerDelay) {
  digitalWrite(RightTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(RightTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(RightTrigPin, LOW);
  rightDuration = pulseIn(RightEchoPin, HIGH);
  rightDistanceCm = rightDuration * SOUND_VELOCITY / 2;
  rightDistancePercentage = 100 - ((rightDistanceCm / storageCapacity) * 100);
  Serial.print("Right Distance (cm): ");
  Serial.println(rightDistanceCm);
  Serial.print("Right Distance : ");
  Serial.print(rightDistancePercentage);
  Serial.println("%");

  myData.id = BOARD_ID;
  myData.Data = 1;
  myData.x = rightDistanceCm;
  myData.y = rightDistancePercentage;

  // Send message via ESP-NOW
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  //    lastTime = millis();
  //  }

  delay(pauseTime);
}

void checkLeftSensor() {

  //  if ((millis() - lastTime) > timerDelay) {
  digitalWrite(LeftTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(LeftTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(LeftTrigPin, LOW);
  leftDuration = pulseIn(LeftEchoPin, HIGH);
  leftDistanceCm = leftDuration * SOUND_VELOCITY / 2;
  leftDistancePercentage = 100 - ((leftDistanceCm / storageCapacity) * 100);
  Serial.print("Left Distance (cm): ");
  Serial.println(leftDistanceCm);
  Serial.print("Left Distance : ");
  Serial.print(leftDistancePercentage);
  Serial.println("%");
  // Set values to send
  myData.id = BOARD_ID;
  myData.Data = 2;
  myData.x = leftDistanceCm;
  myData.y = leftDistancePercentage;

  // Send message via ESP-NOW
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  //    lastTime = millis();
  //  }

  delay(pauseTime);
}
