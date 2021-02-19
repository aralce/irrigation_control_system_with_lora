/*
 * zonesGatekeeper.c
 *
 *  Created on: 9 ene. 2021
 *      Author: Ariel
 */

/* Includes -------------------------------------------------------------------------------------------------*/
#include "../config/configuration.h"
#include "zonesGatekeeper_public.h"

/*Public variables ((((temporal hasta armar zonesGatekeeper_internal))))*/


/*Private variables --------------------------------------------------------------------------------------------*/

/*Functions structures*/
/*structure to send orders to the zonesGatekeeper task*/
static struct{
  enum_zone_operationType operationType;
  uint32_t ID;
  void *data;
} IN_struct;



/*Private function declarations ---------------------------------------------------------------------------------*/
/*auxiliary functions*/
/*setGenericPublicOrder:
 * It's is used to make the internal behavior of some zonesGatekeeper public functions
 * ExtraDataToSend is used when the function needs to send another parameter */
static bool setGenericPublicOrder(uint32_t ID, enum_zone_operationType operationType, void *data);


/*Public function definition -----------------------------------------------------------------------------------*/

bool zonesGatekeeper_getName(uint32_t ID) /*returns True if OK, else False*/
{
  return setGenericPublicOrder(ID, getName, NULL);
}


bool zonesGatekeeper_setName(uint32_t ID, char *zoneName) /*returns True if OK, else False.*/
{
  static char emptyString[] = ""; /*To be send in case of error*/

  if( strlen(zoneName) <= MAX_NAME_SIZE)
     return setGenericPublicOrder(ID, setName, (void *) sensorName);
  else
    return setGenericPublicOrder(ID, setName, (void *) emptyString);
}


bool zonesGatekeeper_getState(uint32_t ID) /*returns True if OK, else False*/
{
  return setGenericPublicOrder(ID, getState, NULL);
}


bool zonesGatekeeper_setState(uint32_t ID, enum_zone_state state ) /*returns True if OK, else False*/
{
  enum_zone_state zoneState_private = OFF;
  zoneState_private = state;
  return setGenericPublicOrder(ID, setState, (void *) sensorType_private);
}



/*Private function definition ------------------------------------------------------------------------------------*/

/*auxiliary functions*/
/*setGenericPublicOrder:
 * It's is used to generate the internal behavior of some sensorsGatekeeper public functions
 * ExtraDataToSend is used when the function needs to send another parameter */
static bool setGenericPublicOrder(uint32_t ID, enum_zone_operationType operationType, void *data)
{
  bool operationSuccess = false;

  /*loads the IN_struct to send it with the qSensorsGatekeeperIN*/
  IN_struct.operationType = operationType;
  IN_struct.ID = ID;
  IN_struct.data = data;

  if( osMessageQueuePut(qSensorsGatekeeperINHandle, &IN_struct, 0, 500) == osOK ) /*sends the IN structure*/
    operationSuccess = true;


  return operationSuccess;
}

