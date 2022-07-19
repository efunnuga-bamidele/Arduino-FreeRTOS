#include <Arduino_FreeRTOS.h>

#define RED    6
#define YELLOW 7
#define BLUE   8

typedef int TaskProfiler;

TaskProfiler REDLEDProfiler;
TaskProfiler BLUELEDProfiler;
TaskProfiler YELLOWLEDProfiler;

void setup() {
  Serial.begin(9600);
  xTaskCreate(redLedControllerTask, "RED LED Task", 128, NULL, 1, NULL);
  xTaskCreate(blueLedControllerTask, "BLUE LED Task", 128, NULL, 1, NULL);
  xTaskCreate(yellowLedControllerTask, "YELLOW LED Task", 128, NULL, 1, NULL);
}

//Task functions
void redLedControllerTask(void *pvParameters)
{
  pinMode(RED, OUTPUT);
  while(1)
  {
    REDLEDProfiler++;
    Serial.print("RedLEDProfiler : ");
    Serial.println(REDLEDProfiler);
    digitalWrite(RED, digitalRead(RED)^1);
  }
}

void blueLedControllerTask(void *pvParameters)
{
  pinMode(BLUE, OUTPUT);
  while(1)
  {
    BLUELEDProfiler++;
    Serial.print("BlueLEDProfiler : ");
    Serial.println(BLUELEDProfiler);
    digitalWrite(BLUE, digitalRead(BLUE)^1);
  }
}

void yellowLedControllerTask(void *pvParameters)
{
  pinMode(YELLOW, OUTPUT);
  while(1)
  {
    YELLOWLEDProfiler++;
    Serial.print("YellowLEDProfiler : ");
    Serial.println(YELLOWLEDProfiler);
    digitalWrite(YELLOW, digitalRead(YELLOW)^1);
  }
}

void loop() {}
