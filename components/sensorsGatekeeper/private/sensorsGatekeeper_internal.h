/*
 * sensorsGatekeeper_internal.h
 *
 *  Created on: 18 ene. 2021
 *      Author: ariel
 */

#ifndef SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_
#define SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_

/* Includes -------------------------------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <cmsis_os.h>
#include <stdlib.h>


/* Typedefs -------------------------------------------------------------------------------------------------*/

/**
 * @brief Clasifica el tipo de sensor
 *
 * Se reservan 16bits para clasificar los sensores.
 * Cada sensor deberá tener su driver asociado para tomar
 * las mediciones del sensor.
 *
 * Si desea crear un driver, su función prototipo debe ser debe ser: TBD.
 */

typedef enum{ /*max quantity of sensors types: 255 */
  noSensor,
  analog,
  rs485,
  sensorType_ENUM_END
}enum_sensorType;


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
  ERROR
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


/*Public functions --------------------------------------------------------------------------------------------*/

void sensorsGatekeeper_internal_init( osMessageQueueId_t *qSensorsGatekeeperINHandle, osMessageQueueId_t *qSensorsGatekeeperOUTHandle );

#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_INTERNAL_H_ */
