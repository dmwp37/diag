#ifndef _DG_PLATFORM_DEFS_H
#define _DG_PLATFORM_DEFS_H
/*==================================================================================================

    Module Name:  dg_platform_defs.h

    General Description: Contains defines for basic types for the platform

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

/*==================================================================================================
                                               MACROS
==================================================================================================*/
#ifndef TRUE
    #define TRUE   1 
    #define FALSE  0 
#endif

#define DG_COMPILE_UNUSED(x) (void)(x)

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* Do not redefine if we are already define those types */
typedef unsigned char        UINT8;      /**< Unsigned 8 bit integer */
typedef signed char          INT8;       /**< Signed 8 bit integer */
typedef unsigned short int   UINT16;     /**< Unsigned 16 bit integer */
typedef signed short int     INT16;      /**< Signed 16 bit integer */
typedef unsigned  int        UINT32;     /**< Unsigned 32 bit integer */
typedef signed int           INT32;      /**< Signed 32 bit integer */
typedef signed long long     INT64;      /**< Signed 64 bit integer */
typedef unsigned long long   UINT64;     /**< Unsigned 64 bit integer */
typedef unsigned char        BOOLEAN;    /**< Boolean type */
typedef BOOLEAN              BOOL;       /**< Boolean type */
typedef unsigned short       W_CHAR;     /**< Wide char */

/*==================================================================================================
                                      GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus  
}       
#endif

#endif

