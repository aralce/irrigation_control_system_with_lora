/*
 * sensorsGatekeeper_internal.h
 *
 *  Created on: 18 ene. 2021
 *      Author: ariel
 */

#ifndef SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_
#define SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_

/* Includes -------------------------------------------------------------------------------------------------*/
#include "sensorsGatekeeper_drivers.h"


/* Typedefs -------------------------------------------------------------------------------------------------*/

typedef enum
{
  withoutOperation,
  getName,
  setName,
  getType,
  setType,
  getMeasureInterval,
  setMeasureInterval,
  getMeasure
}enum_sensor_operationType;

typedef enum
{
  OK,
  INVALID_ID,
  INVALID_NAME,
  INVALID_SENSOR_TYPE,
  INVALID_MEASURE_INTERVAL,

}enum_sensorOrderError;


/**
 * @brief Estructura básica de medición
 *
 * Se utiliza para los datos leídos de la medición. Tendrá siempre un valor de medida
 * y un timestamp medido en ticks del sistema.
 */
typedef struct{
	uint32_t value;
	uint32_t timestamp;
}struct_sensor_measure;


/*Public variables --------------------------------------------------------------------------------------------*/
uint32_t timerID[MAX_SENSOR_ID];


/*Public functions --------------------------------------------------------------------------------------------*/

void sensorsGatekeeper_internal_init( osMessageQueueId_t *qSensorsGatekeeperINHandle, osMessageQueueId_t *qSensorsGatekeeperOUTHandle );

void sensorsGatekeeper_processIncomingOrders(void);

bool sensorsGatekeeper_takeMeasure( uint32_t sensorID ); /*returns true if there is no error*/

#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_ */
