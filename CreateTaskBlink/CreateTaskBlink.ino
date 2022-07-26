#include <Arduino_FreeRTOS.h>

#define redLED    6
#define blueLED   7
#define yellowLED 8

typedef int TaskProfiler;

TaskProfiler RedLEDProfiler;
TaskProfiler BlueLEDProfiler;
TaskProfiler YellowLEDProfiler;


void setup()
{
  Serial.begin(9600);

  xTaskCreate(redLedControllerTask,"Red LED Task", 100, NULL, 1, NULL);
  xTaskCreate(blueLedControllerTask, "Blue LED Task", 100, NULL, 1, NULL);
  xTaskCreate(yellowLedControllerTAsk, "Yellow LED Task", 100, NULL, 1, NULL);

  
  
}

void redLedControllerTask(void *pvParameters)
{
  pinMode(redLED, OUTPUT);
  while(1)
  {
    Serial.println("Red LED Task"); 
  }
}

void blueLedControllerTask(void *pvParameters)
{
  pinMode(blueLED, OUTPUT);
  while(1)
  {
     Serial.println("Blue LED Task"); 
  }
}

void yellowLedControllerTAsk(void *pvPatameters)
{
  pinMode(yellowLED, OUTPUT);
  while(1)
  {
     Serial.println("Yellow LED Task"); 
  }
}

void loop()
{
  
}
