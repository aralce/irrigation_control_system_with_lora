/*
 * sensorsGatekeer.c
 *
 *  Created on: 9 ene. 2021
 *      Author: Ariel
 */

/* Includes -------------------------------------------------------------------------------------------------*/
#include "sensorsGatekeeper_public.h"
#include "../config/configuration.h"

/*Private variables --------------------------------------------------------------------------------------------*/

/*Pointer to qSensorsGatekeeperIN queue*/
static osMessageQueueId_t *qSensorsGatekeeperIN_private = NULL;

/*Functions structures*/
/*structure to send orders to the sensorsGatekeeper task*/
static struct{
  enum_sensor_operationType operationType;
  uint32_t ID;
  void *data;
} IN_struct;


/*auxiliary functions*/
/*setGenericPublicOrder:
 * It's is used to make the internal behavior of some sensorsGatekeeper public functions
 * ExtraDataToSend is used when the function needs to send another parameter */
static bool setGenericPublicOrder(uint32_t ID, enum_sensor_operationType operationType, void *data);


/*Public function definition -----------------------------------------------------------------------------------*/
void sensorsGatekeeper_init(osMessageQueueId_t *qSensorsGatekeeperINHandle, osMessageQueueId_t *qSensorsGatekeeperOUTHandle)
{
  qSensorsGatekeeperIN_private = qSensorsGatekeeperINHandle; /*saves the pointer to the queue to work with it*/
  sensorsGatekeeper_internal_init( qSensorsGatekeeperIN_private, qSensorsGatekeeperOUTHandle); /*initializes the internal structure of the sensorsGatekeeper*/

}


bool sensorsGatekeeper_getName(uint32_t ID) /*returns True if OK, else False*/
{
  return setGenericPublicOrder(ID, getName, NULL);
}


bool sensorsGatekeeper_setName(uint32_t ID, char *sensorName) /*returns True if OK, else False.*/
{
  static char emptyString[] = ""; /*To be send in case of error*/

  if( strlen(sensorName) <= MAX_NAME)
     return setGenericPublicOrder(ID, setName, (void *) sensorName);
  else
    return setGenericPublicOrder(ID, setName, (void *) emptyString);
}


bool sensorsGatekeeper_getType(uint32_t ID) /*returns True if OK, else False*/
{
  return setGenericPublicOrder(ID, getType, NULL);
}


bool sensorsGatekeeper_setType(uint32_t ID, enum_sensorType sensorType) /*returns True if OK, else False*/
{
  enum_sensorType sensorType_private = noSensor;
  sensorType_private = sensorType;
  return setGenericPublicOrder(ID, setType, (void *) sensorType_private);
}


bool sensorsGatekeeper_getMeasureInterval(uint32_t ID) /*returns True if OK, else False*/
{
  return setGenericPublicOrder(ID, getMeasureInterval, NULL);
}


bool sensorsGatekeeper_setMeasureInterval(uint32_t ID, uint32_t measureInterval ) /*returns True if OK, else False*/
{
  uint32_t measureInterval_private = 0;
  measureInterval_private = measureInterval;
  return setGenericPublicOrder(ID, setMeasureInterval, (void *) measureInterval_private );
}


bool sensorsGatekeeper_getMeasure(uint32_t ID, struct_sensor_measure *measure ) /*returns True if OK, else False*/
{
  struct_sensor_measure measure_private = {0, 0};
  measure_private = measure;
  return setGenericPublicOrder(ID, getMeasure, (void *) measure_private );

}



/*Private function definition ------------------------------------------------------------------------------------*/

/*auxiliary functions*/
/*setGenericPublicOrder:
 * It's is used to make the internal behavior of some sensorsGatekeeper public functions
 * ExtraDataToSend is used when the function needs to send another parameter */
static bool setGenericPublicOrder(uint32_t ID, enum_sensor_operationType operationType, void *data)
{
  bool operationSuccess = false;

  /*loads the IN_struct to send it with the qSensorsGatekeeperIN*/
  IN_struct.operationType = operationType;
  IN_struct.ID = ID;
  IN_struct.data = data;

  if( osMessageQueuePut(*qSensorsGatekeeperIN_private, &IN_struct, 0, 500) ) /*sends the IN structure*/
    operationSuccess = true;


  return operationSuccess;
}
