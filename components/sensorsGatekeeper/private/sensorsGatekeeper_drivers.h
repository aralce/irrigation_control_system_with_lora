/*
 * sensorsGatekeeper_drivers.h
 *
 *  Created on: 3 feb. 2021
 *      Author: ariel
 */

#ifndef SENSORSGATEKEEPER_SENSORSGATEKEEPER_DRIVERS_H_
#define SENSORSGATEKEEPER_SENSORSGATEKEEPER_DRIVERS_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <cmsis_os.h>
#include <stdlib.h>


/*Public typedefs -------------------------------------------------------------------------------------------------*/

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



/*Public array of functions ---------------------------------------------------------------------------------------*/

extern uint32_t (*driverArray[3])( uint32_t sensorID );


#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_DRIVERS_H_ */
