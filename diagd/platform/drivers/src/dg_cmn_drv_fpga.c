/*==================================================================================================

    Module Name:  dg_cmn_drv_fpga.c

    General Description: Implements the FPGA common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <string.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_fpga.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FPGA_driver
@{
implementation of the FPGA driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_FPGA_MAX_REG 64
#define DG_CMN_DRV_FPGA_MAX_MEM 1024

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/* simulate the FPGA register set */
static DG_CMN_DRV_FPGA_REG_T dg_cmn_drv_fpga_reg[DG_CMN_DRV_FPGA_MAX_REG];

/* simulate the FPGA memory set */
static UINT8 dg_cmn_drv_fpga_mem[DG_CMN_DRV_FPGA_MAX_MEM];

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads specified FPGA register

@param[in]  addr     - The offset of the FPGA register
@param[out] reg_data - The data read from the register

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_read_reg(DG_CMN_DRV_FPGA_REG_ADDR_T addr, DG_CMN_DRV_FPGA_REG_T* reg_data)
{
    BOOL ret = FALSE;

    if (addr < DG_CMN_DRV_FPGA_MAX_REG)
    {
        *reg_data = dg_cmn_drv_fpga_reg[addr];
        DG_DBG_TRACE("Read FPGA register: address=0x%04x, value=0x%08x", addr, *reg_data);

        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read FPGA register failed: address=0x%04x", addr);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes specified FPGA register

@param[in] addr     - The offset of the FPGA register
@param[in] reg_data - The data write to the register
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_write_reg(DG_CMN_DRV_FPGA_REG_ADDR_T addr, DG_CMN_DRV_FPGA_REG_T reg_data)
{
    BOOL ret = FALSE;

    if (addr < DG_CMN_DRV_FPGA_MAX_REG)
    {
        dg_cmn_drv_fpga_reg[addr] = reg_data;
        DG_DBG_TRACE("Write FPGA register address=0x%04x, value=0x%08x", addr, reg_data);

        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Write FPGA register failed: address=0x%04x", addr);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Reads data from the FPGA memory

@param[in]  address   - The FPGA memory address to read from
@param[in]  read_len  - The amount of data to read in bytes
@param[out] read_data - The data read from the FPGA memory

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_read_mem(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                              DG_CMN_DRV_FPGA_MEM_SIZE_T read_len,
                              UINT8*                     read_data)
{
    BOOL ret = FALSE;

    DG_CMN_DRV_FPGA_MEM_SIZE_T len = 0;

    while (addr < DG_CMN_DRV_FPGA_MAX_MEM)
    {
        if (len == read_len)
        {
            DG_DBG_TRACE("Read FPGA memory address=0x%08x, length=0x%04x", addr, read_len);
            ret = TRUE;
            break;
        }
        *read_data = dg_cmn_drv_fpga_mem[addr];

        addr++;
        read_data++;
        len++;
    }

    if (!ret)
    {
        DG_DRV_UTIL_set_error_string("Read FPGA memory failed: address=0x%08x, length=0x%04x",
                                     addr, read_len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes data to the FPGA memory

@param[in]  address    - The FPGA memory address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the FPGA memory
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_write_mem(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                               DG_CMN_DRV_FPGA_MEM_SIZE_T write_len,
                               UINT8*                     write_data)
{
    BOOL ret = FALSE;

    DG_CMN_DRV_FPGA_MEM_SIZE_T len = 0;

    while (addr < DG_CMN_DRV_FPGA_MAX_MEM)
    {
        if (len == write_len)
        {
            DG_DBG_TRACE("Write FPGA memory address=0x%08x, length=0x%04x", addr, write_len);
            ret = TRUE;
            break;
        }
        dg_cmn_drv_fpga_mem[addr] = *write_data;

        addr++;
        write_data++;
        len++;
    }

    if (!ret)
    {
        DG_DRV_UTIL_set_error_string("Write FPGA memory failed: address=0x%08x, length=0x%04x",
                                     addr, write_len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief DMA Read from the FPGA memory

@param[in]  address   - The FPGA memory address to read from
@param[in]  read_len  - The amount of data to read in bytes
@param[out] read_data - The data read from the FPGA memory

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_dma_recv(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                              DG_CMN_DRV_FPGA_MEM_SIZE_T read_len,
                              UINT8*                     read_data)
{
    BOOL ret = FALSE;

    if ((addr + read_len) < DG_CMN_DRV_FPGA_MAX_MEM)
    {
        DG_DBG_TRACE("DMA recv FPGA memory address=0x%08x, length=0x%04x", addr, read_len);
        memcpy(read_data, &dg_cmn_drv_fpga_mem[addr], read_len);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("DMA recv FPGA memory failed: address=0x%08x, length=0x%04x",
                                     addr, read_len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief DMA Write to the FPGA memory

@param[in]  address    - The FPGA memory address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the FPGA memory
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_dma_send(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                              DG_CMN_DRV_FPGA_MEM_SIZE_T write_len,
                              UINT8*                     write_data)
{
    BOOL ret = FALSE;

    if ((addr + write_len) < DG_CMN_DRV_FPGA_MAX_MEM)
    {
        DG_DBG_TRACE("DMA send FPGA memory address=0x%08x, length=0x%04x", addr, write_len);
        memcpy(&dg_cmn_drv_fpga_mem[addr], write_data, write_len);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("DMA send FPGA memory failed: address=0x%08x, length=0x%04x",
                                     addr, write_len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Execute the FPAG self-test

@param[in] sub_test - which self-test to execute

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_self_test(DG_CMN_DRV_FPGA_SELF_TEST_T sub_test)
{
    BOOL ret = FALSE;

    switch (sub_test)
    {
    case DG_CMN_DRV_FPGA_ST_REG:
    case DG_CMN_DRV_FPGA_ST_MEM_BUS:
    case DG_CMN_DRV_FPGA_ST_MEM_WALK:
    case DG_CMN_DRV_FPGA_ST_MEM_FIX:
    case DG_CMN_DRV_FPGA_ST_MEM_RAND:
        DG_DBG_TRACE("FPGA self test, sub_test=0x%02x", sub_test);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid FPGA self test, sub_test=0x%02x", sub_test);
        break;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set the FPAG mode

@param[in] mode - the desired FPGA mode

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_set_mode(DG_CMN_DRV_FPGA_MODE_T mode)
{
    BOOL ret = FALSE;

    switch (mode)
    {
    case DG_CMN_DRV_FPGA_NOMAL_MODE:
    case DG_CMN_DRV_FPGA_BYPASS_MODE:
    case DG_CMN_DRV_FPGA_INTER_LOOP_MODE:
    case DG_CMN_DRV_FPGA_OUTER_LOOP_MODE:
    case DG_CMN_DRV_FPGA_PRBS_MODE:
        DG_DBG_TRACE("set FPGA mode, mode=0x%02x", mode);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid FPGA mode, mode=0x%02x", mode);
        break;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Download the FPAG image

@param[in] image - the desired FPGA image type

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_download_image(DG_CMN_DRV_FPGA_IMAGE_T image)
{
    BOOL ret = FALSE;

    switch (image)
    {
    case DG_CMN_DRV_FPGA_PRODUCT_IMAGE:
    case DG_CMN_DRV_FPGA_TEST_IMAGE:
    case DG_CMN_DRV_FPGA_MAX_IMAGE:
        DG_DBG_TRACE("download FPGA image=0x%02x", image);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid FPGA download image, image=0x%02x", image);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

