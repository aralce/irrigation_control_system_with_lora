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
extern osMessageQueueId_t qSensorsGetekeeperOUTHandle;
extern const osMessageQueueAttr_t qSensorsGetekeeperOUT_attributes;

/*to work with timer callback*/
uint32_t timerID[MAX_SENSOR_ID];

/*timer_sensor0 */
extern osTimerId_t timer_sensor0Handle;
extern const osTimerAttr_t timer_sensor0_attributes;

/*timer_sensor1 */
extern osTimerId_t timer_sensor1Handle;
extern const osTimerAttr_t timer_sensor1_attributes;

/*timer_sensor2 */
extern osTimerId_t timer_sensor2Handle;
extern const osTimerAttr_t timer_sensor2_attributes;

/*timer_sensor3 */
extern osTimerId_t timer_sensor3Handle;
extern const osTimerAttr_t timer_sensor3_attributes;

/*timer_sensor4 */
extern osTimerId_t timer_sensor4Handle;
extern const osTimerAttr_t timer_sensor4_attributes;

/*timer_sensor5 */
extern osTimerId_t timer_sensor5Handle;
extern const osTimerAttr_t timer_sensor5_attributes;

/*timer_sensor6 */
extern osTimerId_t timer_sensor6Handle;
extern const osTimerAttr_t timer_sensor6_attributes;



/*Public functions --------------------------------------------------------------------------------------------*/

void sensorsGatekeeper_init( );

void sensorsGatekeeper_task( void* parameters);

void sensorsGatekeeper_takeMeasure( void* sensorID );

#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_ */
