#ifndef _DG_CMN_DRV_I2C_H_
#define _DG_CMN_DRV_I2C_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_i2c.h

    General Description: This file provides driver interface for I2C test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup I2C_driver
@{

@par
Provide APIs for I2C
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** I2C Buses */
typedef UINT8 DG_CMN_DRV_I2C_BUS_T;

/** I2C Device Address */
typedef UINT8 DG_CMN_DRV_I2C_ADDR_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads data from a given address on the given I2C bus

@param[in]  bus       - The I2C bus to read from
@param[in]  address   - The device address to read from
@param[in]  read_len  - The amount of data to read in bytes
@param[out] read_data - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                             UINT16 read_len, UINT8* read_data);

/*=============================================================================================*//**
@brief Writes data to a given address on the given I2C bus

@param[in]  bus        - The I2C bus to write to
@param[in]  address    - The device address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_write_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                              UINT16 write_len, UINT8* write_data);

/*=============================================================================================*//**
@brief Writes data to a given address on the given I2C bus and then reads data from that address

@param[in]  bus        - The I2C bus to write to
@param[in]  address    - The device address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the device
@param[in]  read_len   - The amount of data to read in bytes
@param[out] read_data  - The data read from the device

@note
- Both the Write then Read must occur within a single I2C transaction
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_write_read_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                                   UINT16 write_len, UINT8* write_data,
                                   UINT16 read_len, UINT8* read_data);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

