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
#include "../../config/configuration.h"

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

/*tskSensorsGatekeeper*/
extern osThreadId_t tskSensorsGatekeeperHandle;
extern const osThreadAttr_t tskSensorsGatekeeper_attributes;

/*Queues for INPUT and OUTPUT data*/
/*qSensorsGatekeeperIN */
extern osMessageQueueId_t qSensorsGatekeeperINHandle;
extern const osMessageQueueAttr_t qSensorsGatekeeperIN_attributes;
/*qSensorsGetekeeperOUT */
extern osMessageQueueId_t qSensorsGatekeeperOUTHandle;
extern const osMessageQueueAttr_t qSensorsGetekeeperOUT_attributes;

/*to work with timer callback*/
uint32_t timerID[MAX_SENSOR_ID];

/*Sensors Timers*/
extern osTimerId_t timer_sensorHandle[MAX_SENSOR_ID];
extern const osTimerAttr_t timer_sensor_attributes[MAX_SENSOR_ID];




/*Public functions --------------------------------------------------------------------------------------------*/

void sensorsGatekeeper_init( );

void sensorsGatekeeper_task( void* parameters);

void sensorsGatekeeper_takeMeasure( void* sensorID );

#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_ */
