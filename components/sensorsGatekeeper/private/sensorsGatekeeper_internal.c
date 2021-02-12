/*
 * sensorsGatekeeper_internal.c
 *
 *  Created on: 18 ene. 2021
 *      Author: Ariel
 */

#include "sensorsGatekeeper_internal.h"
#include "cmsis_os.h"
#include "../../config/configuration.h"


/*Public variables --------------------------------------------------------------------------------------------*/

/*tskSensorsGatekeeper */
osThreadId_t tskSensorsGatekeeperHandle;
const osThreadAttr_t tskSensorsGatekeeper_attributes = {
  .name = "tskSensorsGatekeeper",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 1024 * 4
};

/*Queues for INPUT and OUTPUT data*/
/*qSensorsGatekeeperIN */
osMessageQueueId_t qSensorsGatekeeperINHandle;
const osMessageQueueAttr_t qSensorsGatekeeperIN_attributes = {
  .name = "qSensorsGatekeeperIN"
};
/*qSensorsGetekeeperOUT */
osMessageQueueId_t qSensorsGetekeeperOUTHandle;
const osMessageQueueAttr_t qSensorsGetekeeperOUT_attributes = {
  .name = "qSensorsGetekeeperOUT"
};

/*Sensors Timers*/
/*timer_sensor0 */
osTimerId_t timer_sensor0Handle;
const osTimerAttr_t timer_sensor0_attributes = {
  .name = "timer_sensor0"
};
/*timer_sensor1 */
osTimerId_t timer_sensor1Handle;
const osTimerAttr_t timer_sensor1_attributes = {
  .name = "timer_sensor1"
};
/*timer_sensor2 */
osTimerId_t timer_sensor2Handle;
const osTimerAttr_t timer_sensor2_attributes = {
  .name = "timer_sensor2"
};
/*timer_sensor3 */
osTimerId_t timer_sensor3Handle;
const osTimerAttr_t timer_sensor3_attributes = {
  .name = "timer_sensor3"
};
/*timer_sensor4 */
osTimerId_t timer_sensor4Handle;
const osTimerAttr_t timer_sensor4_attributes = {
  .name = "timer_sensor4"
};
/*timer_sensor5 */
osTimerId_t timer_sensor5Handle;
const osTimerAttr_t timer_sensor5_attributes = {
  .name = "timer_sensor5"
};
/*timer_sensor6 */
osTimerId_t timer_sensor6Handle;
const osTimerAttr_t timer_sensor6_attributes = {
  .name = "timer_sensor6"
};



/*Private typedefs ----------------------------------------------------------------------------------------------*/

typedef struct {
  uint32_t ID; /*sensor identifier*/
  char name[MAX_NAME_SIZE + 1];
  enum_sensorType type;
  uint32_t measureInterval; /*time between measures. Time in seconds*/
  struct_sensor_measure measure;
}struct_sensor;

/*Pointer to function */
typedef uint32_t (*sensor_measureDriverfunction_t)( uint32_t sensorID );




/*Private variables ---------------------------------------------------------------------------------------------*/

/*List of sensors */
static struct_sensor sensor[MAX_SENSOR_ID + 1];

/*Receiving orders structure*/
static struct{
  enum_sensor_operationType operationType;
  uint32_t ID;
  void *data;
} IN_struct;

static struct{
  enum_sensor_operationType operationType;
  uint32_t ID;
  void *data;
  enum_sensorOrderError errorType;
} OUT_struct;

/*Pointer to measure function driver*/
sensor_measureDriverfunction_t takeMeasureFromSensor[MAX_SENSOR_ID];



/*Private function definitions ----------------------------------------------------------------------------------*/
inline uint32_t HAL_secondsToTicks( uint32_t seconds ){
  uint32_t returnValue = 0;
  uint32_t comparator = ~0; /*sets comparator to the max value permitted to a uint32_t variable*/
  uint64_t ticks = 0;

  ticks = seconds*HAL_getTickFreq();

  /*checks if there is overflow*/
  if( ticks >= (uint64_t)comparator )
    returnValue = comparator;
  else
    returnValue = ticks;

  return returnValue;
}

static bool sensorsGatekeeper_setTimerPeriod( uint32_t sensorID ); /*returns true if sensorID is valid*/



/*Public function definition -----------------------------------------------------------------------------------*/

void sensorsGatekeeper_init( )
{

  uint32_t sensorNumber = 0;
  char tmp[17] = ""; /*helps to load the sensor number on the sensor name.*/

  /* creation of tskSensorsGatekeeper */
    tskSensorsGatekeeperHandle = osThreadNew(sensorsGatekeeper_task, NULL, &tskSensorsGatekeeper_attributes);

  /* Create the queue(s) */
    /* creation of qSensorsGatekeeperIN */
    qSensorsGatekeeperINHandle = osMessageQueueNew (32, sizeof(void *), &qSensorsGatekeeperIN_attributes);
    /* creation of qSensorsGetekeeperOUT */
    qSensorsGetekeeperOUTHandle = osMessageQueueNew (16, sizeof(void *), &qSensorsGetekeeperOUT_attributes);

  /*creates the timers for the sensors*/
    /* creation of timer_sensor0 */
    timer_sensor0Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[0], &timer_sensor0_attributes);
    /* creation of timer_sensor1 */
    timer_sensor1Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[1], &timer_sensor1_attributes);
    /* creation of timer_sensor2 */
    timer_sensor2Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[2], &timer_sensor2_attributes);
    /* creation of timer_sensor3 */
    timer_sensor3Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[3], &timer_sensor3_attributes);
    /* creation of timer_sensor4 */
    timer_sensor4Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[4], &timer_sensor4_attributes);
    /* creation of timer_sensor5 */
    timer_sensor5Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[5], &timer_sensor5_attributes);
    /* creation of timer_sensor6 */
    timer_sensor6Handle = osTimerNew(sensorsGatekeeper_takeMeasure, osTimerPeriodic, (void*) timerID[6], &timer_sensor6_attributes);


  /*Initializes the sensor structures*/
  for(sensorNumber = 0; sensorNumber <=MAX_SENSOR_ID; sensorNumber++)
    {
      sensor[sensorNumber].ID = sensorNumber;

      /*loads the sensor name*/
      strcpy( (char* volatile) sensor[sensorNumber].name, "Sensor ");
      itoa( sensorNumber, tmp, 10 );
      strcat( (char* volatile) sensor[sensorNumber].name, (const char* volatile) tmp );

      sensor[sensorNumber].type = noSensor;
      sensor[sensorNumber].measureInterval = 0;
      sensor[sensorNumber].measure.timestamp = 0;
      sensor[sensorNumber].measure.value = 0;

      /*sets the driver function for the sensor*/
      takeMeasureFromSensor[sensorNumber] = driverArray[INITIAL_SENSOR_DRIVER];

      /*initializes the timer ID*/
      timerID[sensorNumber] = sensorNumber;
    }
}



void sensorsGatekeeper_task(void* parameters)
{
  static const char *sensorName = NULL; /*to return the sensor name of the sensor used*/
  static enum_sensorType sensorType = sensorType_ENUM_END; /*ENUM_END is used to know the number of elements and as a signal of error*/
  static uint32_t measureInterval = 0; /*to return the sensor interval of the sensor used*/
  static const struct_sensor_measure *sensorMeasure = NULL; /*to query the sensor measure*/

  osMessageQueueGet(qSensorsGatekeeperINHandle, &IN_struct, 0, osWaitForever ); /*waits until an order arrives*/

  /*processes the command*/
  switch(IN_struct.operationType)
  {
    case getName:
      /*executes the order*/
      if( IN_struct.ID <= MAX_SENSOR_ID ) /*checks the ID*/
      {
        sensorName = (const char *) sensor[IN_struct.ID].name;
        OUT_struct.errorType = OK;
      }
      else
      {
        sensorName = NULL; /*if there is an error*/
        OUT_struct.errorType = INVALID_ID;
      }

      /*loads the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) sensorName;
      break;


    case setName:
      sensorName = (const char *)IN_struct.data;

      /*executes the order*/
      if( IN_struct.ID <= MAX_SENSOR_ID ) /*the nameSize was checked on sensorsGatekeeper_public*/
      {
        if( strcmp(sensorName, "") != 0 ) /*sensorsGatekeeper_public sends an empty string when the name is not valid*/
        {
          strcpy( sensor[IN_struct.ID].name, sensorName);
          OUT_struct.errorType = OK;
        }
        else
        {
          sensorName = NULL; /*if there is an error*/
          OUT_struct.errorType = INVALID_NAME;
        }
      }
      else
      {
        sensorName = NULL; /*if there is an error*/
        OUT_struct.errorType = INVALID_ID;
      }

      /*loads the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) sensorName;
      break;

    case getType:
      if( IN_struct.ID <= MAX_SENSOR_ID ) /*checks the ID*/
      {
        sensorType = sensor[IN_struct.ID].type;
        OUT_struct.errorType = OK;
      }
      else
      {
        sensorType = sensorType_ENUM_END; /*if there is an error*/
        OUT_struct.errorType = INVALID_ID;
      }

      /*load the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) sensorType;
      break;


    case setType:
      sensorType = (enum_sensorType) IN_struct.data;

      /*executes the order*/
      if( IN_struct.ID <= MAX_SENSOR_ID ) /*checks the ID*/
      {
        if(sensorType < sensorType_ENUM_END) /*checks if the sensorType is valid*/
        {
          sensor[IN_struct.ID].type = sensorType;
          OUT_struct.errorType = OK;

          /*sets the corresponding measure function*/
          takeMeasureFromSensor[IN_struct.ID] = driverArray[(uint32_t) sensorType];
        }
        else
          OUT_struct.errorType = INVALID_SENSOR_TYPE;
      }
      else
        OUT_struct.errorType = INVALID_ID;

      /*load the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) sensorType;
      break;


    case getMeasureInterval:
      if( IN_struct.ID <= MAX_SENSOR_ID ) /*checks the ID*/
      {
        measureInterval = sensor[IN_struct.ID].measureInterval;
        OUT_struct.errorType = OK;
      }
      else
        OUT_struct.errorType = INVALID_ID;

      /*loads the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) measureInterval;
      break;


    case setMeasureInterval:
      measureInterval = (uint32_t) IN_struct.data;

      /*checks the entry data*/
      if( IN_struct.ID <= MAX_SENSOR_ID )
      {
        if( measureInterval <= MAX_MEASURE_INTERVAL )
        {
          sensor[IN_struct.ID].measureInterval = measureInterval;
          OUT_struct.errorType = OK;
        }
        else
          OUT_struct.errorType = INVALID_MEASURE_INTERVAL;
      }
      else
        OUT_struct.errorType = INVALID_ID;

      /*loads the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) measureInterval;
      break;


    case getMeasure:
      if( IN_struct.ID <= MAX_SENSOR_ID )
      {
        sensorMeasure = &sensor[IN_struct.ID].measure;
        OUT_struct.errorType = OK;
      }
      else
      {
        sensorMeasure = NULL; /*if there is an error*/
        OUT_struct.errorType = INVALID_ID;
      }

      /*loads the data onto the output structures*/
      OUT_struct.operationType = IN_struct.operationType;
      OUT_struct.ID = IN_struct.ID;
      OUT_struct.data = (void *) sensorMeasure;
      break;


    default:
      break;

  }

}


/*Used by the timers to take the measure from the sensor*/
void sensorsGatekeeper_takeMeasure( void* sensorID )
{

  /*checks the sensorID*/
  if( (uint32_t)sensorID <= MAX_SENSOR_ID )
  {
    sensor[(uint32_t)sensorID].measure.value = takeMeasureFromSensor[(uint32_t)sensorID]((uint32_t)sensorID);
    /*sensor measure.Timestamp: TBD*/

    /*loads the data onto the output structures*/
    OUT_struct.operationType = getMeasure;
    OUT_struct.ID = IN_struct.ID;
    OUT_struct.data = (void *) sensor[(uint32_t)sensorID].measure.value;

    /*sends the data to the datalogger*/
    if( osMessageQueuePut(qSensorsGetekeeperOUTHandle, &OUT_struct, 0, 500) != osOK)
      /*Error Message*/;

  }
}


/*Private function definitions ---------------------------------------------------------------------------------------*/

static bool sensorsGatekeeper_setTimerPeriod( uint32_t sensorID ) /*returns true if sensorID is valid*/
{

}

