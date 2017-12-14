//Hardware init if needed global and comments about task reserved hardware:
//A4 / A5 RTC via i2c
//A0 water lvl sensor1
//A1 water lvl sensor 2
//P13 Feeder (MPP)
//P12 Pump
//P11 / P10 Valve
//P9 Turn on/off the whole water management system
//P8 LED
  
//Libraries
#include <Arduino_FreeRTOS.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <stdio.h>
#include <Wire.h>
#include "RTClib.h"

//Global variables
RTC_DS1307 RTC;
DateTime currentTime;
bool mustFeed = true;
bool toDrain = true;
bool waitSecondFinished = true;

#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6
#define SUNDAY 7

#define WaterPump 12
#define FillAquarium 10
#define EmptyAquarium 11
#define TurnOnWaterSystem 9
#define AquariumWaterLevel A0
#define WasteTankWaterLevel A1

EventGroupHandle_t rtc_event_group;
int LIGHT = (1 << 0);
int FEED = (1 << 1);
int WATER = (1 << 2);
int SENSOR = (1 << 3);

bool sensorNotOk = 0;

//Forward task functions declarations
void TaskSensorsRead(void *pvParameters);
void TaskFeed(void *pvParameters);
void TaskWater(void *pvParameters);
void TaskRTC(void *pvParameters);

//Forward general function declarations
void rtc_get_time();
void feed_time_manager();
void sensor_read_time_manager();
void water_time_manager();
void TEST_TIME();


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  // Initializing pins for all needed purposes
  pinMode(FillAquarium, OUTPUT);
  pinMode(EmptyAquarium, OUTPUT);
  pinMode(WaterPump, OUTPUT);
  pinMode(TurnOnWaterSystem, OUTPUT);
  pinMode(AquariumWaterLevel, INPUT);
  pinMode(WasteTankWaterLevel, INPUT);
  
  if (!RTC.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  rtc_event_group = xEventGroupCreate();
  //portMAX_DELAY makes a mutex wait forever
  if( rtc_event_group == NULL )
  {
    /* The event group was not created because there was insufficient
    FreeRTOS heap available. */
    Serial.println("Problem rtc_event_group");
  }
  
  //Tasks init:
  xTaskCreate(&TaskSensorsRead, (const portCHAR *)"SensorRead", 128, NULL, 1, NULL);
  xTaskCreate(&TaskFeed, (const portCHAR *)"Feed", 128, NULL, 3, NULL);
  xTaskCreate(&TaskWater, (const portCHAR *)"Water", 128, NULL, 3, NULL);
  xTaskCreate(&TaskRTC, (const portCHAR *)"RTC", 128, NULL, 1, NULL);
}

void loop()
{
// DO NOT ADD CODE HERE. Things are done ONYL in Tasks.
}

void TaskSensorsRead(void *pvParameters)
{
  (void) pvParameters;
  //@TODO Add initial task setup

   for (;;)
  {
    //Calling this task once every 30s
    xEventGroupWaitBits(rtc_event_group, SENSOR, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("SENSOR");
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //@TODO one function per sensor to read
    //Set results in gobal variables (as small as possible) so they can be used by other tasks
  }
}

void TaskFeed(void *pvParameters)
{
  (void) pvParameters;
  //@TODO Add initial task setup

   for (;;)
  {
    //Calling task once every 60s
    xEventGroupWaitBits(rtc_event_group, FEED, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("FEED");
  }
}

void TaskWater(void *pvParameters)
{
  (void) pvParameters;
  //P12 Pump
  //P11 / P10 Valve
  //P9 
  //@TODO Add initial task setup
  int itrt_a = 0;
  uint32_t MAXTIME = 30; // Number of seconds to keep the pump running
  bool okToFill = 1;
  
  for (;;)
  {
    xEventGroupWaitBits(rtc_event_group, WATER, pdFALSE, pdTRUE, portMAX_DELAY);
    //@TODO log when draining begun and calculate when to stop
    //@TODO log when starting to fill and calculate when to stop
    digitalWrite(TurnOnWaterSystem, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(EmptyAquarium, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(WaterPump, HIGH);
    
    for(itrt_a = 0; itrt_a <= MAXTIME; itrt_a++)
    {
      vTaskDelay(500 / portTICK_PERIOD_MS);
      if(sensorNotOk)
      {
        digitalWrite(WaterPump, LOW);
        digitalWrite(TurnOnWaterSystem, LOW);
        okToFill = 0;
        xEventGroupClearBits(rtc_event_group, WATER);
        break;
      }
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
    if(okToFill)
    { 
      digitalWrite(WaterPump, LOW);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(EmptyAquarium, LOW);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(FillAquarium, HIGH);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(WaterPump, HIGH);

      // Check if sensor is ok while pumping water for given time
      for(itrt_a = 0; itrt_a <= MAXTIME; itrt_a++)
      {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if(sensorNotOk)
        {
          digitalWrite(WaterPump, LOW);
          digitalWrite(TurnOnWaterSystem, LOW);
          okToFill = 0;
          xEventGroupClearBits(rtc_event_group, WATER);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    }
    digitalWrite(WaterPump, LOW);
    digitalWrite(TurnOnWaterSystem, LOW);
    xEventGroupClearBits(rtc_event_group, WATER);
  }
}

void TaskRTC(void *pvParameters)
{
  (void) pvParameters;
  
  for(;;)
  {
    rtc_get_time();
    feed_time_manager();
    water_time_manager();
    sensor_read_time_manager();
//    TEST_TIME();
  }
}

//Functions used by tasks:

void rtc_get_time()
{
  //Update time inside the global variable currentTime
  currentTime = RTC.now();
}

void feed_time_manager()
{
  /*This functions sets the feeding every 5 hours*/
  if(currentTime.hour() == 9)
  {
    if(mustFeed)
    {
      xEventGroupSetBits(rtc_event_group, FEED);
      mustFeed = false;
    }
  }
  if(currentTime.hour() == 10)
  {
    mustFeed = true;
  }
  
  if(currentTime.hour() == 14)
  {
    if(mustFeed)
    {
      xEventGroupSetBits(rtc_event_group, FEED);
      mustFeed = false;
    }
  }
  if(currentTime.hour() == 15)
  {
   mustFeed = true;
  }
  
  if(currentTime.hour() == 19)
  {
    if(mustFeed)
    {
      xEventGroupSetBits(rtc_event_group, FEED);
      mustFeed = false;
    }
  }
  if(currentTime.hour() == 20)
  {
    mustFeed = true;
  }
}

void water_time_manager()
{
  if((currentTime.dayOfWeek() == WEDNESDAY && toDrain) && (currentTime.hour() == 11))
  {
    xEventGroupSetBits(rtc_event_group, WATER);
    toDrain = false;
  }
  if(currentTime.dayOfWeek() != WEDNESDAY)
  {
    toDrain = true;
  }
}

void TEST_TIME()
{
  Serial.print(currentTime.year(), DEC);
  Serial.print('/');
  Serial.print(currentTime.month(), DEC);
  Serial.print('/');
  Serial.print(currentTime.day(), DEC);
  Serial.print(' ');
  Serial.print(currentTime.hour(), DEC);
  Serial.print(':');
  Serial.print(currentTime.minute(), DEC);
  Serial.print(':');
  Serial.print(currentTime.second(), DEC);
  Serial.println(); 
}

void sensor_read_time_manager()
{
  if((currentTime.second() == 25 || currentTime.second() == 55) && waitSecondFinished)
  {
    xEventGroupSetBits(rtc_event_group, SENSOR);
    waitSecondFinished = false;
  }
  if(currentTime.second() == 26 || currentTime.second() ==  56)
  {
    waitSecondFinished = true;
  }
}

