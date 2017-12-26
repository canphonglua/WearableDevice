/******************************************************************************

 @file  simple_gatt_profile.c

 @brief This file contains the Simple GATT profile sample GATT service profile
        for use with the BLE sample application.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2010-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_00_31
 Release Date: 2016-06-16 18:57:29
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include "bcomdef.h"
#include "osal.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "pedometer.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        17

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 pedometerServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_SERV_UUID), HI_UINT16(Pedometer_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
CONST uint8 pedometerchar1UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_CHAR1_UUID), HI_UINT16(Pedometer_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
CONST uint8 pedometerchar2UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_CHAR2_UUID), HI_UINT16(Pedometer_CHAR2_UUID)
};

// Characteristic 3 UUID: 0xFFF3
CONST uint8 pedometerchar3UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_CHAR3_UUID), HI_UINT16(Pedometer_CHAR3_UUID)
};

// Characteristic 4 UUID: 0xFFF4
CONST uint8 pedometerchar4UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_CHAR4_UUID), HI_UINT16(Pedometer_CHAR4_UUID)
};

// Characteristic 5 UUID: 0xFFF5
CONST uint8 pedometerchar5UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(Pedometer_CHAR5_UUID), HI_UINT16(Pedometer_CHAR5_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static pedometerCBs_t *pedometer_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute
static CONST gattAttrType_t pedometerService = { ATT_BT_UUID_SIZE, pedometerServUUID };


// Simple Profile Characteristic 1 Properties
static uint8 pedometerChar1Props = GATT_PROP_READ;

// Characteristic 1 Value
static uint8 pedometerChar1 = 0;

// Simple Profile Characteristic 1 User Description
static uint8 pedometerChar1UserDesp[17] = "Accel Range";


// Simple Profile Characteristic 2 Properties
static uint8 pedometerChar2Props = GATT_PROP_READ;

// Characteristic 2 Value
static uint8 pedometerChar2 = 0;

// Simple Profile Characteristic 2 User Description
static uint8 pedometerChar2UserDesp[17] = "Step Count";


// Simple Profile Characteristic 3 Properties
static uint8 pedometerChar3Props = GATT_PROP_READ;

// Characteristic 3 Value
static uint8 pedometerChar3 = 0;

// Simple Profile Characteristic 3 User Description
static uint8 pedometerChar3UserDesp[17] = "Calo Burn";


// Simple Profile Characteristic 4 Properties
static uint8 pedometerChar4Props = GATT_PROP_READ;

// Characteristic 4 Value
static uint8 pedometerChar4 = 0;

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t *pedometerChar4Config;
                                        
// Simple Profile Characteristic 4 User Description
static uint8 pedometerChar4UserDesp[17] = "Distance";


// Simple Profile Characteristic 5 Properties
static uint8 pedometerChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
static uint8 pedometerChar5[Pedometer_CHAR5_LEN] = { 0, 0, 0, 0, 0 };

// Simple Profile Characteristic 5 User Description
static uint8 pedometerChar5UserDesp[17] = "Characteristic 5";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t pedometerAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&pedometerService            /* pValue */
  },

    // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &pedometerChar1Props 
    },

      // Characteristic Value 1
      { 
        { ATT_BT_UUID_SIZE, pedometerchar1UUID },
        GATT_PERMIT_READ, 
        0, 
        &pedometerChar1 
      },

      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        pedometerChar1UserDesp 
      },      

    // Characteristic 2 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &pedometerChar2Props 
    },

      // Characteristic Value 2
      { 
        { ATT_BT_UUID_SIZE, pedometerchar2UUID },
        GATT_PERMIT_READ, 
        0, 
        &pedometerChar2 
      },

      // Characteristic 2 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        pedometerChar2UserDesp 
      },           
      
    // Characteristic 3 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &pedometerChar3Props 
    },

      // Characteristic Value 3
      { 
        { ATT_BT_UUID_SIZE, pedometerchar3UUID },
        GATT_PERMIT_READ, 
        0, 
        &pedometerChar3 
      },

      // Characteristic 3 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        pedometerChar3UserDesp 
      },

    // Characteristic 4 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &pedometerChar4Props 
    },

      // Characteristic Value 4
      { 
        { ATT_BT_UUID_SIZE, pedometerchar4UUID },
        GATT_PERMIT_READ, 
        0, 
        &pedometerChar4 
      },

      // Characteristic 4 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&pedometerChar4Config 
      },
      
      // Characteristic 4 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        pedometerChar4UserDesp 
      },
      
    // Characteristic 5 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &pedometerChar5Props 
    },

      // Characteristic Value 5
      { 
        { ATT_BT_UUID_SIZE, pedometerchar5UUID },
        GATT_PERMIT_AUTHEN_READ, 
        0, 
        pedometerChar5 
      },

      // Characteristic 5 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        pedometerChar5UserDesp 
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t pedometer_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t pedometer_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Simple Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t pedometerCBs =
{
  pedometer_ReadAttrCB,     // Read callback function pointer
  NULL,                     // Write callback function pointer
  NULL                      // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Pedometer_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Pedometer_AddService( uint32 services )
{
  uint8 status;

  // Allocate Client Characteristic Configuration table
  pedometerChar4Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            linkDBNumConns );
  if ( pedometerChar4Config == NULL )
  {     
    return ( bleMemAllocError );
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, pedometerChar4Config );
  
  if ( services & Pedometer_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( pedometerAttrTbl, 
                                          GATT_NUM_ATTRS( pedometerAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &pedometerCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      Pedometer_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Pedometer_RegisterAppCBs( pedometerCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pedometer_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      Pedometer_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Pedometer_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case Pedometer_CHAR1:
      if ( len == sizeof ( uint8 ) ) 
      {
        pedometerChar1 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case Pedometer_CHAR2:
      if ( len == sizeof ( uint8 ) ) 
      {
        pedometerChar2 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case Pedometer_CHAR3:
      if ( len == sizeof ( uint8 ) ) 
      {
        pedometerChar3 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case Pedometer_CHAR4:
      if ( len == sizeof ( uint8 ) ) 
      {
        pedometerChar4 = *((uint8*)value);
        
        // // See if Notification has been enabled
        // GATTServApp_ProcessCharCfg( pedometerChar4Config, &pedometerChar4, FALSE,
        //                             pedometerAttrTbl, GATT_NUM_ATTRS( pedometerAttrTbl ),
        //                             INVALID_TASK_ID, pedometer_ReadAttrCB );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case Pedometer_CHAR5:
      if ( len == Pedometer_CHAR5_LEN ) 
      {
        VOID memcpy( pedometerChar5, value, Pedometer_CHAR5_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      Pedometer_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Pedometer_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case Pedometer_CHAR1:
      *((uint8*)value) = pedometerChar1;
      break;

    case Pedometer_CHAR2:
      *((uint8*)value) = pedometerChar2;
      break;      

    case Pedometer_CHAR3:
      *((uint8*)value) = pedometerChar3;
      break;  

    case Pedometer_CHAR4:
      *((uint8*)value) = pedometerChar4;
      break;

    case Pedometer_CHAR5:
      VOID memcpy( value, pedometerChar5, Pedometer_CHAR5_LEN );
      break;      
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          pedometer_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t pedometer_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case Pedometer_CHAR1_UUID:
      case Pedometer_CHAR2_UUID:
      case Pedometer_CHAR3_UUID:
      case Pedometer_CHAR4_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;

      case Pedometer_CHAR5_UUID:
        *pLen = Pedometer_CHAR5_LEN;
        VOID memcpy( pValue, pAttr->pValue, Pedometer_CHAR5_LEN );
        break;
        
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      pedometer_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t pedometer_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case Pedometer_CHAR1_UUID:
      case Pedometer_CHAR3_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          *pCurValue = pValue[0];

          if( pAttr->pValue == &pedometerChar1 )
          {
            notifyApp = Pedometer_CHAR1;        
          }
          else
          {
            notifyApp = Pedometer_CHAR3;           
          }
        }
             
        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && pedometer_AppCBs && pedometer_AppCBs->pfnPedometerChange )
  {
    pedometer_AppCBs->pfnPedometerChange( notifyApp );  
  }
  
  return ( status );
}

/*********************************************************************
*********************************************************************/