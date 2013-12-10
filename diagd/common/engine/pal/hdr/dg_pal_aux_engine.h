#ifndef _DG_PAL_AUX_ENGINE_H
#define _DG_PAL_AUX_ENGINE_H
/*==================================================================================================

    Module Name:  dg_pal_aux_engine.h

    General Description: DIAG PAL - Auxiliary DIAG Engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Engine_PAL
@{
*/

/** @addtogroup aux_engine
@{

@par
Allows communication with a AUX diag engine

*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/
/** Aux Engine init status type */
typedef enum
{
    DG_PAL_AUX_ENGINE_INIT_SUCCESS     = 0,
    DG_PAL_AUX_ENGINE_INIT_FAIL        = 1,
    DG_PAL_AUX_ENGINE_INIT_NOT_PRESENT = 2
} DG_PAL_AUX_ENGINE_INIT_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                      GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Initializes the communcation interface with the specified auxiliary DIAG engine

@param[in] aux_id - The aux engine to init

@return Status of initialization

@note
 - If no auxiliary DIAG engine is present, DG_PAL_AUX_ENGINE_INIT_NOT_PRESENT must be returned
 - This function should NOT support an aux id of DG_AUX_ENGINE_AUX_ID_INIT_ALL
*//*==============================================================================================*/
DG_PAL_AUX_ENGINE_INIT_T DG_PAL_AUX_ENGINE_init(int aux_id);

/*=============================================================================================*//**
@brief close the communication interface with the specified auxiliary DIAG engine

@param[in] aux_id - The aux engine to close

@note
 - This function should NOT support an aux id of DG_AUX_ENGINE_AUX_ID_INIT_ALL
*//*==============================================================================================*/
void DG_PAL_AUX_ENGINE_close(int aux_id);

/*=============================================================================================*//**
@brief Reads the specified number of bytes from the specified auxiliary DIAG engine

@param[in]  aux_id        - The aux engine to read from
@param[in]  bytes_to_read - The number of bytes to read
@param[out] data          - Data read

@return TRUE = success, FALSE = failure

@note
 - The read is synchronous, the function will block until the requested number of bytes are read
*//*==============================================================================================*/
BOOL DG_PAL_AUX_ENGINE_read(int aux_id, UINT32 bytes_to_read, UINT8 *data);

/*=============================================================================================*//**
@brief Writes the specified number of bytes to the specified auxiliary DIAG engine

@param[in]  aux_id         - The aux engine to write to
@param[in]  bytes_to_write - The number of bytes to write
@param[out] data           - Data to write

@return TRUE = success, FALSE = failure

@note
 - The write is synchronous, the function will block until the requested number of bytes are written
*//*==============================================================================================*/
BOOL DG_PAL_AUX_ENGINE_write(int aux_id, UINT32 bytes_to_write, UINT8 *data);


/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif

