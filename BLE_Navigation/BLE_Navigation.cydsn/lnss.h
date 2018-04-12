/*******************************************************************************
* File Name: lnss.h
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

#if !defined(LNSS_H)
#define LNSS_H

#include "main.h"


#define LS_NTF              (0x01u)
#define NV_NTF              (0x02u)
#define NV_EN               (0x04u)
#define INV_PAR             (0x08u)
#define LNS_COUNT           (0x0004u)
#define LNS_TIME            (2u)    /* Counts of 0.5 second */
    
/* L&N Feature characteristic flags */
#define CYBLE_LNS_FT_IS     (0x00000001u)   /* Instantaneous Speed Supported */
#define CYBLE_LNS_FT_TD     (0x00000002u)   /* Total Distance Supported */
#define CYBLE_LNS_FT_LC     (0x00000004u)   /* Location Supported */
#define CYBLE_LNS_FT_EL     (0x00000008u)   /* Elevation Supported */
#define CYBLE_LNS_FT_HD     (0x00000010u)   /* Heading Supported */
#define CYBLE_LNS_FT_RT     (0x00000020u)   /* Rolling Time Supported */
#define CYBLE_LNS_FT_UTC    (0x00000040u)   /* UTC Time Supported */
#define CYBLE_LNS_FT_RD     (0x00000080u)   /* Remaining Distance Supported */
#define CYBLE_LNS_FT_RVD    (0x00000100u)   /* Remaining Vertical Distance Supported */
#define CYBLE_LNS_FT_ETA    (0x00000200u)   /* Estimated Time of Arrival Supported */
#define CYBLE_LNS_FT_NBS    (0x00000400u)   /* Number of Beacons in Solution Supported */
#define CYBLE_LNS_FT_NBV    (0x00000800u)   /* Number of Beacons in View Supported */
#define CYBLE_LNS_FT_TFF    (0x00001000u)   /* Time to First Fix Supported */
#define CYBLE_LNS_FT_EHE    (0x00002000u)   /* Estimated Horizontal Position Error Supported */
#define CYBLE_LNS_FT_EVE    (0x00004000u)   /* Estimated Vertical Position Error Supported */
#define CYBLE_LNS_FT_HDP    (0x00008000u)   /* Horizontal Dilution of Precision Supported */
#define CYBLE_LNS_FT_VDP    (0x00010000u)   /* Vertical Dilution of Precision Supported */
#define CYBLE_LNS_FT_LSM    (0x00020000u)   /* Location and Speed Characteristic Content Masking Supported */
#define CYBLE_LNS_FT_FRS    (0x00040000u)   /* Fix Rate Setting Supported */
#define CYBLE_LNS_FT_ES     (0x00080000u)   /* Elevation Setting Supported */
#define CYBLE_LNS_FT_PS     (0x00100000u)   /* Position Status Supported */

/* Location and Speed characteristic "flags" */
#define CYBLE_LNS_LS_FLG_IS     (0x0001u)   /* Instantaneous Speed Present */
#define CYBLE_LNS_LS_FLG_TD     (0x0002u)   /* Total Distance Present */
#define CYBLE_LNS_LS_FLG_LC     (0x0004u)   /* Location Present */
#define CYBLE_LNS_LS_FLG_EL     (0x0008u)   /* Elevation Present */
#define CYBLE_LNS_LS_FLG_HD     (0x0010u)   /* Heading Present */
#define CYBLE_LNS_LS_FLG_RT     (0x0020u)   /* Rolling Time Present */
#define CYBLE_LNS_LS_FLG_UTC    (0x0040u)   /* UTC Time Present */
#define CYBLE_LNS_LS_FLG_PS     (0x0180u)   /* Position Status mask */
#define CYBLE_LNS_LS_FLG_PS_NP  (0x0000u)   /* Position Status: No Position */
#define CYBLE_LNS_LS_FLG_PS_OK  (0x0080u)   /* Position Status: Position Ok */
#define CYBLE_LNS_LS_FLG_PS_EP  (0x0100u)   /* Position Status: Estimated Position */
#define CYBLE_LNS_LS_FLG_PS_LK  (0x0180u)   /* Position Status: Last Known Position */
#define CYBLE_LNS_LS_FLG_SDF    (0x0200u)   /* Speed and Distance format */
#define CYBLE_LNS_LS_FLG_ES     (0x0400u)   /* Elevation Source */
#define CYBLE_LNS_LS_FLG_HS     (0x0800u)   /* Heading Source */

typedef struct
{
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hours;
    uint8  minutes;
    uint8  seconds;
}CYBLE_DATE_TIME_T;

typedef struct
{
    uint16 flags;
    uint16 instSpd;     /* Instantaneous Speed */
    uint32 totalDst;    /* Total Distance */
    int32  latitude;    /* Location - Latitude */
    int32  longitude;   /* Location - Longitude */
    int32  elevation;   /* Elevation */
    uint16 heading;     /* Heading */
    uint8  rollTime;    /* Rolling Time */
    CYBLE_DATE_TIME_T utcTime; /* UTC Time */
}CYBLE_LNS_LS_T;

/* Opcode of Record Access Control Point characteristic value type */
typedef enum
{
    CYBLE_LNS_CP_OPC_SCV = 1u,  /* Set Cumulative Value (Parameter: Cumulative Value) */
    CYBLE_LNS_CP_OPC_MLS,       /* Mask Location and Speed Characteristic Content (Parameter: Content Mask) */
    CYBLE_LNS_CP_OPC_NC,        /* Navigation Control (Parameter: Control value) */
    CYBLE_LNS_CP_OPC_NRS,       /* Request Number of Routes (Parameter: None) */
    CYBLE_LNS_CP_OPC_RNM,       /* Request Name of Route (Parameter: Route Number) */
    CYBLE_LNS_CP_OPC_SR,        /* Select Route (Parameter: Route Number) */
    CYBLE_LNS_CP_OPC_SFR,       /* Set Fix Rate (Parameter: Fix Rate) */
    CYBLE_LNS_CP_OPC_SE,        /* Set Elevation (Parameter: Elevation) */
    CYBLE_LNS_CP_OPC_RC = 32u,  /* Response Code */
} CYBLE_LNS_CP_OPC_T;

/* Navigation Control OpCode parameter value type */
typedef enum
{
    CYBLE_LNS_CP_OPC_NC_STOP,   /* Stop Notification of Navigation characteristic. Stop Navigation */
    CYBLE_LNS_CP_OPC_NC_START,  /* Start Notification of Navigation characteristic. 
                                 * Start Navigation to first waypoint on route 
                                 */
    CYBLE_LNS_CP_OPC_NC_PAUSE,  /* Stop Notification of Navigation characteristic. 
                                 * Pause Navigation keeping next waypoint on route 
                                 * in the memory for continuing navigation later 
                                 */
    CYBLE_LNS_CP_OPC_NC_RESUME, /* Start Notification of Navigation characteristic. 
                                 * Continue Navigation from point where navigation 
                                 * was paused to next waypoint on route 
                                 */
    CYBLE_LNS_CP_OPC_NC_SKIP,   /* Notification of Navigation characteristic not affected. 
                                 * Skip Way point: disregard next waypoint and continue navigation 
                                 * to waypoint following next waypoint on route
                                 */
    CYBLE_LNS_CP_OPC_NC_NEAR    /* Start Notification of Navigation characteristic. 
                                 * Select Nearest Waypoint on Route: measure distance to all
                                 * waypoints on route, and start navigation to closest or 
                                 * optimal waypoint on route (left to implementation) and 
                                 * from there to waypoints following next waypoint along route 
                                 */
} CYBLE_LNS_CP_OPC_NC_T;

typedef enum
{
    CYBLE_LNS_CP_RSP_SUCCESS = 1u,  /* Response for successful operation */
    CYBLE_LNS_CP_RSP_UNSPRT_OPC,    /* Response if unsupported Op Code is received */
    CYBLE_LNS_CP_RSP_INV_PAR,       /* Response if Parameter received does not meet requirements
                                     * of service or is outside of supported range of Sensor.
                                     */
    CYBLE_LNS_CP_RSP_FAIL           /* Operation Failed */
} CYBLE_LNS_CP_RSP_T;

/* Navigation characterictic "flags" */
#define CYBLE_LNS_NV_FLG_RD     (0x0001u)   /* Remaining Distance Present */
#define CYBLE_LNS_NV_FLG_RVD    (0x0002u)   /* Remaining Vertical Distance Present */
#define CYBLE_LNS_NV_FLG_EAT    (0x0004u)   /* Estimated Time of Arrival Present */
#define CYBLE_LNS_NV_FLG_PS     (0x0018u)   /* Position Status mask */
#define CYBLE_LNS_NV_FLG_PS_NP  (0x0000u)   /* Position Status: No Position */
#define CYBLE_LNS_NV_FLG_PS_OK  (0x0008u)   /* Position Status: Position Ok */
#define CYBLE_LNS_NV_FLG_PS_EP  (0x0010u)   /* Position Status: Estimated Position */
#define CYBLE_LNS_NV_FLG_PS_LK  (0x0018u)   /* Position Status: Last Known Position */
#define CYBLE_LNS_NV_FLG_RT     (0x0020u)   /* Heading Source */
#define CYBLE_LNS_NV_FLG_UTC    (0x0040u)   /* Navigation Indicator Type */
#define CYBLE_LNS_NV_FLG_SDF    (0x0080u)   /* Waypoint Reached */
#define CYBLE_LNS_NV_FLG_ES     (0x0100u)   /* Destination Reached */

typedef struct
{
    uint16 flags;
    uint16 bearing;     /* Bearing */
    uint16 heading;     /* Heading */
    uint32 rDst;        /* Remaining Distance */
    int32  rvDst;       /* Remaining Vertical Distance */
    CYBLE_DATE_TIME_T eaTime; /* Estimated Time of Arrival */
}CYBLE_LNS_NV_T;


/***************************************
*      Function prototypes
***************************************/
void LnsInit(void);
void LnsCallBack(uint32 event, void* eventParam);
void LnsProcess(void);
void LnsNtf(void);

/***************************************
*      External data references
***************************************/
extern CYBLE_LNS_LS_T ls;
extern CYBLE_LNS_NV_T nv;


#endif /* LNSS_H  */

/* [] END OF FILE */
