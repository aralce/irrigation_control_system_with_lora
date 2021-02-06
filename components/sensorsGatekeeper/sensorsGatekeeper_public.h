/*
 * sensorsGatekeeper.h
 *
 *  Created on: 9 ene. 2021
 *      Author: Ariel
 */

#ifndef SENSORSGATEKEEPER_SENSORSGATEKEEPER_PUBLIC_H_
#define SENSORSGATEKEEPER_SENSORSGATEKEEPER_PUBLIC_H_

/* Includes ------------------------------------------------------------------*/
#include "private/sensorsGatekeeper_internal.h"


/*Public functions -------------------------------------------------------------*/


/**
 * @brief Inicializa el Gatekeeper.
 *
 * La inicialización acondiciona las estructuras internas de los sensores.
 * @see struct_sensor.
 *
 * @param Handler de la queue de entrada al Gatekeeper.
 */
void sensorsGatekeeper_init(osMessageQueueId_t *qSensorsGatekeeperINHandle, osMessageQueueId_t *qSensorsGatekeeperOUTHandle);


/**
 * @brief Obtiene el nombre del Sensor
 *
 * Esta función se utiliza para ordenar al sensorsGatekeeper que entregue el nombre
 * del sensor deseado.
 * El nombre del sensor será enviado al Datalogger.
 *
 * @param ID Identificador del sensor. Para los sensores físicos debe estar comprendido
 * entre 0 y 6. Siendo 0 el "Sensor 1" y 6 el "Sensor 7"
 *
 * @retval True Devuelve true si la orden se envía con normalidad al sensorsGatekeeper.
 * @retval False Devuelve false si hubo un problema. Probablemente el ID no sea válido.
 *
 */
bool sensorsGatekeeper_getName(uint32_t ID); /*returns True if OK, else False*/
bool sensorsGatekeeper_setName(uint32_t ID, char *sensorName); /*returns True if OK, else False. Máx characters 16*/
bool sensorsGatekeeper_getType(uint32_t ID); /*returns True if OK, else False*/
bool sensorsGatekeeper_setType(uint32_t ID, enum_sensorType sensorType); /*returns True if OK, else False*/
bool sensorsGatekeeper_getMeasureInterval(uint32_t ID); /*returns True if OK, else False*/
bool sensorsGatekeeper_setMeasureInterval(uint32_t ID, uint32_t measureInterval ); /*returns True if OK, else False*/
bool sensorsGatekeeper_getMeasure(uint32_t ID, struct_sensor_measure *measure ); /*returns True if OK, else False*/

#endif /* SENSORSGATEKEEPER_SENSORSGATEKEEPER_PUBLIC_H_ */
