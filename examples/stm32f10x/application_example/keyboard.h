/**
* \file keyboard.h
* \brief
*
*
*
* \internal
* \par Modification history
* - 1.00 2015-11-11 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#ifdef __cplusplus
extern “C” {
#endif

/** Includes -----------------------------------------------------------------*/
/** Exported typedef ---------------------------------------------------------*/
typedef enum{
    KEYBOARD_KeyVal_0,
    KEYBOARD_KeyVal_1,
    KEYBOARD_KeyVal_2,
    KEYBOARD_KeyVal_3,
    KEYBOARD_KeyVal_4,
    KEYBOARD_KeyVal_5,
    KEYBOARD_KeyVal_6,
    KEYBOARD_KeyVal_7,
    KEYBOARD_KeyVal_8,
    KEYBOARD_KeyVal_9,
    KEYBOARD_KeyVal_plus,
    KEYBOARD_KeyVal_minus,
    KEYBOARD_KeyVal_multi,
    KEYBOARD_KeyVal_div,
    KEYBOARD_KeyVal_equal,
    KEYBOARD_KeyVal_esc,

} KEYBOARD_KeyVal_Enum;
/** Exported MACRO -----------------------------------------------------------*/
/** Exported Functions -------------------------------------------------------*/

void KEYBOARD_Init(void);

#ifdef __cplusplus
}
#endif

#endif  /* __XXX_H */

/* end of file */
