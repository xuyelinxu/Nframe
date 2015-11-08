/**
* \file     nfdebug.h
* \brief
*
* \internal
* \par Modification history
* - 1.00 2015-8-17 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NFDEBUG_H_
#define _NFDEBUG_H_

/** Includes -----------------------------------------------------------------*/
#include "nframe_common.h"


/** Exported typedef --------------------------------------------------------**/


typedef void (*NFDEBUG_CommandDealFunc)(uint8_t     ParmCount,
                                        char      **Parm );

/* NFDEBUG_CommandDef */
typedef const struct {
    NFDEBUG_CommandDealFunc     CommandDealFunc;
    uint8_t                     CommandStrLength;
    char                       *CommandStr;
} NFDEBUG_CommandItemDef;

typedef const struct {
    uint8_t                     CommandNum;
    NFDEBUG_CommandItemDef     *CommandItems;
} NFDEBUG_CommandDef;


/** Exported MACRO -----------------------------------------------------------*/

#define NFDEBUG_COMMAND_LIST(n)                                             \
    NFDEBUG_CommandItemDef NFDEBUG_CommandListItems[];                      \
    NFDEBUG_CommandDef NFDEBUG_CommandList = {                              \
        n,                                                                  \
        NFDEBUG_CommandListItems                                            \
    };                                                                      \
    NFDEBUG_CommandItemDef NFDEBUG_CommandListItems[n] =

#ifdef  NFDEBUG_ENABLE
#define NFDEBUG_(format,...) printf(""format"\r\n", ##__VA_ARGS__)
#define NFDEBUG(format,...) printf("["__FILE__", %05d] "format"\r\n", __LINE__, ##__VA_ARGS__)
#else
#define NFDEBUG_(format,...)
#define NFDEBUG(format,...)
#endif


/** Exported Functions -------------------------------------------------------*/

void NFDEBUG_Init (void);

void NFDEBUG_Run (void);
void NFDEBUG_ReciveChar (uint8_t ch);

#endif

/*****END OF FILE****/
