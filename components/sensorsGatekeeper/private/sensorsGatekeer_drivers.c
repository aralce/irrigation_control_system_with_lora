/*
 * sensorsGatekeer_drivers.c
 *
 * File to center the drivers logic. The drivers functions can be on external files,
 * but must be referenced here.
 *
 *  Created on: 3 feb. 2021
 *      Author: Ariel
 */

#include "sensorsGatekeeper_drivers.h"


/*Driver Function Declarations -----------------------------------------------------------------------------------*/
/*Those driver functions can be added from external files*/
uint32_t noSensor_measuring( uint32_t sensorID);
uint32_t analogSensor_measuring( uint32_t sensorID );
uint32_t rs485Sensor_measuring( uint32_t sensorID );



/*Driver Pointer Array -------------------------------------------------------------------------------------------*/

uint32_t (*driverArray[3])( uint32_t sensorID ) = { /*driverArray is used to call the corresponding function driver*/
  noSensor_measuring, /*this first function is used to fix the number of functions with the sensorType enumeration*/
  analogSensor_measuring,
  rs485Sensor_measuring
};




/*Driver Function Definitions ------------------------------------------------------------------------------------*/
uint32_t noSensor_measuring( uint32_t sensorID)
{
  return 0;
}

uint32_t analogSensor_measuring( uint32_t sensorID )
{
  return 0;
}

uint32_t rs485Sensor_measuring( uint32_t sensorID )
{
  return 0;
}
