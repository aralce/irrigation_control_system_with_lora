/*
 * sensorsGatekeeper_internal.c
 *
 *  Created on: 18 ene. 2021
 *      Author: Ariel
 */

#include "sensorsGatekeeper_internal.h"
#include "cmsis_os.h"
#include "../../config/configuration.h"

/*Private typedefs --------------------------------------------------------------------------------------------*/

typedef struct {
  uint32_t ID; /*sensor identifier*/
  char name[MAX_NAME_SIZE + 1];
  enum_sensorType type;
  uint32_t measureInterval; /*time between measures. Time in seconds*/
  struct_sensor_measure measure;
}struct_sensor;

/*Pointer to function */
typedef uint32_t (*sensor_measureDriverfunction_t)( uint32_t sensorID );

/*Private variables --------------------------------------------------------------------------------------------*/
static osMessageQueueId_t *qSensorsGatekeeperIN_private = NULL;
static osMessageQueueId_t *qSensorsGatekeeperOUT_private = NULL;

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


/*Public function definition ----------------------------------------------------------------------------------*/

void sensorsGatekeeper_internal_init( osMessageQueueId_t *qSensorsGatekeeperINHandle, osMessageQueueId_t *qSensorsGatekeeperOUTHandle )
{

  uint32_t sensorNumber = 0;
  char tmp[17] = ""; /*helps to load the sensor number on the sensor name.*/

  qSensorsGatekeeperIN_private = qSensorsGatekeeperINHandle; /*saves the pointer to the queue to work with it receiving the orders to execute*/
  qSensorsGatekeeperOUT_private = qSensorsGatekeeperOUTHandle; /*saves the pointer to the queue to work with it sending the messages to the datalogger*/

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



/*Private function declarations -------------------------------------------------------------------------------*/

void sensorsGatekeeper_processIncomingOrders(void)
{
  static const char *sensorName = NULL; /*to return the sensor name of the sensor used*/
  static enum_sensorType sensorType = sensorType_ENUM_END; /*ENUM_END is used to know the number of elements and as a signal of error*/
  static uint32_t measureInterval = 0; /*to return the sensor interval of the sensor used*/
  static const struct_sensor_measure *sensorMeasure = NULL; /*to query the sensor measure*/

  osMessageQueueGet(*qSensorsGatekeeperIN_private, &IN_struct, 0, osWaitForever ); /*waits until an order arrives*/

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
        if(sensorType > sensorType_ENUM_END) /*checks if the sensorType is valid*/
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
bool sensorsGatekeeper_takeMeasure( uint32_t sensorID )
{
  bool returnValue = false;

  /*checks the sensorID*/
  if( sensorID <= MAX_SENSOR_ID )
  {
    returnValue = true;
    sensor[sensorID].measure.value = takeMeasureFromSensor[sensorID]( sensorID );
    /*sensor measure.Timestamp: TBD*/
  }
}
