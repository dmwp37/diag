#ifndef _DG_PAL_CLIENT_PLATFORM_INC_H
#define _DG_PAL_CLIENT_PLATFORM_INC_H
/*==================================================================================================

    Module Name:  dg_pal_client_platform_inc.h

    General Description: DIAG Client PAL - Includes header files needed for platform

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_platform_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                               MACROS
==================================================================================================*/
#ifdef DG_DEBUG
    #define DG_CLIENT_API_TRACE(x ...) do { printf("DIAG API: "x); printf("\n"); } while (0)
#else
    #define DG_CLIENT_API_TRACE(x ...)
#endif

#define DG_CLIENT_API_ERROR(x ...) do { printf("DIAG API ERROR: "x); printf("\n"); } while (0)

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

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
