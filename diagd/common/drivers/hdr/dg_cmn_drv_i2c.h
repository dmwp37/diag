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
/** I2C Buses */
enum
{
    DG_CMN_DRV_I2C_PCH_SMB  = 0x00, /** PCH SMBUS controller  */
    DG_CMN_DRV_I2C_MUX_CPU  = 0x10, /** PAX SMBUS MUX to CPU  */
    DG_CMN_DRV_I2C_MUX_PSU0 = 0x11, /** PAX SMBUS MUX to PSU0 */
    DG_CMN_DRV_I2C_MUX_PSU1 = 0x12, /** PAX SMBUS MUX to PSU1 */
    DG_CMN_DRV_I2C_MUX_WBP  = 0x13, /** PAX SMBUS MUX to WBP  */
    DG_CMN_DRV_I2C_MUX_WTB1 = 0x14, /** PAX SMBUS MUX to WTB1 */
    DG_CMN_DRV_I2C_MUX_WTB2 = 0x15, /** PAX SMBUS MUX to WTB2 */
    DG_CMN_DRV_I2C_MUX_FEB  = 0x16, /** PAX SMBUS MUX to FEB  */
};
typedef UINT8 DG_CMN_DRV_I2C_BUS_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/** I2C Device Address */
typedef UINT8 DG_CMN_DRV_I2C_ADDR_T;

/** I2C Device Offset */
typedef UINT8 DG_CMN_DRV_I2C_OFFSET_T;

/** I2C operation size */
typedef UINT8 DG_CMN_DRV_I2C_SIZE_T;

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
@param[in]  offset    - The device data offset to read from
@param[in]  read_len  - The amount of data to read in bytes
@param[out] read_data - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                             DG_CMN_DRV_I2C_OFFSET_T offset, DG_CMN_DRV_I2C_SIZE_T read_len,
                             UINT8* read_data);

/*=============================================================================================*//**
@brief Writes data to a given address on the given I2C bus

@param[in]  bus        - The I2C bus to write to
@param[in]  address    - The device address to write to
@param[in]  offset     - The device data offset to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_write_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                              DG_CMN_DRV_I2C_OFFSET_T offset, DG_CMN_DRV_I2C_SIZE_T write_len,
                              UINT8* write_data);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_I2C_H_  */

