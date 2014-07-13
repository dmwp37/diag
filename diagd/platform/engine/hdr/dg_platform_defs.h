#ifndef _DG_PLATFORM_DEFS_H
#define _DG_PLATFORM_DEFS_H
/*==================================================================================================

    Module Name:  dg_platform_defs.h

    General Description: Contains defines for basic types for the platform

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                               MACROS
==================================================================================================*/
#ifndef TRUE
    #define TRUE  true
    #define FALSE false
#endif

#define DG_COMPILE_UNUSED(x) (void)(x)
#define DG_ARRAY_SIZE(x)     (sizeof(x) / sizeof(x[0]))

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* Do not redefine if we are already define those types */
typedef uint8_t  UINT8;    /**< Unsigned 8 bit integer */
typedef int8_t   INT8;     /**< Signed 8 bit integer */
typedef uint16_t UINT16;   /**< Unsigned 16 bit integer */
typedef int16_t  INT16;    /**< Signed 16 bit integer */
typedef uint32_t UINT32;   /**< Unsigned 32 bit integer */
typedef int32_t  INT32;    /**< Signed 32 bit integer */
typedef uint64_t UINT64;   /**< Unsigned 64 bit integer */
typedef int64_t  INT64;    /**< Signed 64 bit integer */
typedef bool     BOOL;     /**< Boolean type */
typedef int16_t  W_CHAR;   /**< Wide char */

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

