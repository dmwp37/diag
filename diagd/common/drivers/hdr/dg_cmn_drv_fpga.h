#ifndef _DG_CMN_DRV_FPGA_H_
#define _DG_CMN_DRV_FPGA_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_fpga.h

    General Description: This file provides driver interface for FPGA test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FPGA_driver
@{

@par
Provide APIs for FPGA
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
/** FPGA Register */
typedef UINT32 DG_CMN_DRV_FPGA_REG_T;

/** FPGA Register offset */
typedef UINT16 DG_CMN_DRV_FPGA_REG_ADDR_T;

/** FPGA Memory offset */
typedef UINT32 DG_CMN_DRV_FPGA_MEM_ADDR_T;

/** FPGA Memory offset */
typedef UINT16 DG_CMN_DRV_FPGA_MEM_SIZE_T;

/** FPGA Self-test */
enum
{
    DG_CMN_DRV_FPGA_ST_REG      = 0x00,
    DG_CMN_DRV_FPGA_ST_MEM_BUS  = 0x01,
    DG_CMN_DRV_FPGA_ST_MEM_WALK = 0x02,
    DG_CMN_DRV_FPGA_ST_MEM_FIX  = 0x03,
    DG_CMN_DRV_FPGA_ST_MEM_RAND = 0x04,
};
typedef UINT8 DG_CMN_DRV_FPGA_SELF_TEST_T;

/** FPGA Mode */
enum
{
    DG_CMN_DRV_FPGA_NOMAL_MODE      = 0x00,
    DG_CMN_DRV_FPGA_BYPASS_MODE     = 0x01,
    DG_CMN_DRV_FPGA_INTER_LOOP_MODE = 0x02,
    DG_CMN_DRV_FPGA_OUTER_LOOP_MODE = 0x03,
    DG_CMN_DRV_FPGA_PRBS_MODE       = 0x04,
};
typedef UINT8 DG_CMN_DRV_FPGA_MODE_T;

/** FPGA Image */
enum
{
    DG_CMN_DRV_FPGA_PRODUCT_IMAGE = 0x00,
    DG_CMN_DRV_FPGA_TEST_IMAGE    = 0x01,
    DG_CMN_DRV_FPGA_MAX_IMAGE     = 0x02,
};
typedef UINT8 DG_CMN_DRV_FPGA_IMAGE_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads specified FPGA register

@param[in]  addr     - The offset of the FPGA register
@param[out] reg_data - The data read from the register

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_read_reg(DG_CMN_DRV_FPGA_REG_ADDR_T addr, DG_CMN_DRV_FPGA_REG_T* reg_data);

/*=============================================================================================*//**
@brief Writes specified FPGA register

@param[in] addr     - The offset of the FPGA register
@param[in] reg_data - The data write to the register
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_write_reg(DG_CMN_DRV_FPGA_REG_ADDR_T addr, DG_CMN_DRV_FPGA_REG_T reg_data);

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
                              UINT8*                     read_data);

/*=============================================================================================*//**
@brief Writes data to the FPGA memory

@param[in]  address    - The FPGA memory address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the FPGA memory
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_write_mem(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                               DG_CMN_DRV_FPGA_MEM_SIZE_T write_len,
                               UINT8*                     write_data);

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
                              UINT8*                     read_data);

/*=============================================================================================*//**
@brief DMA Write to the FPGA memory

@param[in]  address    - The FPGA memory address to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the FPGA memory
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_dma_send(DG_CMN_DRV_FPGA_MEM_ADDR_T addr,
                              DG_CMN_DRV_FPGA_MEM_SIZE_T write_len,
                              UINT8*                     write_data);

/*=============================================================================================*//**
@brief Execute the FPAG self-test

@param[in] sub_test - which self-test to execute

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_self_test(DG_CMN_DRV_FPGA_SELF_TEST_T sub_test);

/*=============================================================================================*//**
@brief Set the FPAG mode

@param[in] mode - the desired FPGA mode

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_set_mode(DG_CMN_DRV_FPGA_MODE_T mode);

/*=============================================================================================*//**
@brief Download the FPAG image

@param[in] image - the desired FPGA image type

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_download_image(DG_CMN_DRV_FPGA_IMAGE_T image);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

