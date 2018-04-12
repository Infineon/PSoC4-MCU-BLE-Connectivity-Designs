/*******************************************************************************
* File Name: ancsc.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CGMSS_H)
#define CGMSS_H

#include "main.h"

#define CYBLE_ANCS_MAX_STR_LENGTH (300u)
    
#define CYBLE_ANCS_FLG_START      (0x01u) /* Start ANCS operation */
#define CYBLE_ANCS_FLG_CMD        (0x02u) /* Command */
#define CYBLE_ANCS_FLG_NTF        (0x04u) /* Notification */
#define CYBLE_ANCS_FLG_STR        (0x08u) /* String */
#define CYBLE_ANCS_FLG_ACT        (0x10u) /* Action */
#define CYBLE_ANCS_FLG_RSP        (0x20u) /* Response */

#define CYBLE_ANCS_NS_CNT         (10u)
    
#define CYBLE_ANCS_NS_FLG_SL      (0x01u) /* Silent */
#define CYBLE_ANCS_NS_FLG_IM      (0x02u) /* Important */
#define CYBLE_ANCS_NS_FLG_PE      (0x04u) /* Pre-existing */
#define CYBLE_ANCS_NS_FLG_PA      (0x08u) /* Positive Action */
#define CYBLE_ANCS_NS_FLG_NA      (0x10u) /* Negative Action */
    
typedef enum
{
    CYBLE_ANCS_NS_EVT_ID_ADD, /* Notification Added */
    CYBLE_ANCS_NS_EVT_ID_MOD, /* Notification Modified */
    CYBLE_ANCS_NS_EVT_ID_REM  /* Notification Removed */
}CYBLE_ANCS_NS_EVT_ID_T;

typedef enum
{
    CYBLE_ANCS_NS_CAT_ID_OTH, /* Other */
    CYBLE_ANCS_NS_CAT_ID_INC, /* Incoming Call */
    CYBLE_ANCS_NS_CAT_ID_MIS, /* Missed Call */
    CYBLE_ANCS_NS_CAT_ID_VML, /* Voice mail */
    CYBLE_ANCS_NS_CAT_ID_SOC, /* Social */
    CYBLE_ANCS_NS_CAT_ID_SCH, /* Schedule */
    CYBLE_ANCS_NS_CAT_ID_EML, /* Email */
    CYBLE_ANCS_NS_CAT_ID_NWS, /* News */
    CYBLE_ANCS_NS_CAT_ID_HNF, /* Health and Fitness */
    CYBLE_ANCS_NS_CAT_ID_BNF, /* Business and Finance */
    CYBLE_ANCS_NS_CAT_ID_LOC, /* Location */
    CYBLE_ANCS_NS_CAT_ID_ENT  /* Entertainment */
}CYBLE_ANCS_NS_CAT_ID_T;

typedef enum
{
    CYBLE_ANCS_CP_CMD_ID_GNA, /* Get Notification Attributes */
    CYBLE_ANCS_CP_CMD_ID_GAA, /* Get App Attributes */
    CYBLE_ANCS_CP_CMD_ID_PNA  /* Perform Notification Action */
}CYBLE_ANCS_CP_CMD_ID_T;

typedef enum
{
    CYBLE_ANCS_CP_ACT_ID_POS, /* Positive */
    CYBLE_ANCS_CP_ACT_ID_NEG  /* Negative */
}CYBLE_ANCS_CP_ACT_ID_T;

typedef enum
{
    CYBLE_ANCS_CP_ATT_ID_AID, /* App identifier */
    CYBLE_ANCS_CP_ATT_ID_TTL, /* Title */
    CYBLE_ANCS_CP_ATT_ID_SBT, /* Subtitle */
    CYBLE_ANCS_CP_ATT_ID_MSG, /* Message */
    CYBLE_ANCS_CP_ATT_ID_MSZ, /* Message size */
    CYBLE_ANCS_CP_ATT_ID_DAT, /* Date */
    CYBLE_ANCS_CP_ATT_ID_PAL, /* Positive action label */
    CYBLE_ANCS_CP_ATT_ID_NAL  /* Negative action label */
} CYBLE_ANCS_CP_ATT_ID_T;

typedef struct
{
    CYBLE_ANCS_NS_EVT_ID_T evtId;  /* Event ID */
    uint8                  evtFlg; /* Event Flags */
    CYBLE_ANCS_NS_CAT_ID_T ctgId;  /* Category ID */
    uint8                  ctgCnt; /* Category Count */
    uint32                 ntfUid; /* Notification UID */
}CYBLE_ANCS_NS_T;                  /* Notification Source characteristic structure */

typedef struct
{
    CYBLE_ANCS_CP_CMD_ID_T cmdId;  /* Command ID */
    uint32                 ntfUid; /* Notification UID */
    uint32                 appId;  /* App Identifier */
    CYBLE_ANCS_CP_ATT_ID_T attId;  /* Attribute ID */
    CYBLE_ANCS_CP_ACT_ID_T actId;  /* Action ID */
    CYBLE_ANCS_NS_CAT_ID_T ctgId;  /* Auxiliary field to store Category ID */
}CYBLE_ANCS_CP_T;                  /* Control Point characteristic structure */

typedef struct
{
    CYBLE_ANCS_CP_CMD_ID_T cmdId;  /* Command ID */
    uint32                 ntfUid; /* Notification UID */
    CYBLE_ANCS_CP_ATT_ID_T attId;  /* Attribute ID */
    uint16                 length; /* Length of data */
    uint8*                 data;   /* Pointer to data buffer */
    uint16                 currLength; /* Auxiliary field to store current length */
}CYBLE_ANCS_DS_T;                  /* Data Source characteristic structure */

/***************************************
*      External Function Prototypes
***************************************/

void AncsPrintCharName(CYBLE_ANCS_CHAR_INDEX_T charIndex);
void AncsInit(void);
void AncsCallBack(uint32 event, void* eventParam);
void AncsProcess(void);


/***************************************
*      External data references
***************************************/

extern uint8 ancsFlag;


#endif /* CGMSS_H */

/* [] END OF FILE */
