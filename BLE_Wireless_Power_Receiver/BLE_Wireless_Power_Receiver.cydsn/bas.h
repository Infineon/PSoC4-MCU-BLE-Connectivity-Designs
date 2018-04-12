/*******************************************************************************
* File Name: bas.h
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

#include <project.h>


/***************************************
*  Conditional Compilation Parameters
***************************************/
#define BAS_MEASURE_ENABLE          (1)     /* Set to 1 to enable Battery Level measurement using sequencer ADC */
#define BAS_SIMULATE_ENABLE         (0)     /* Set to 1 to enable Battery Level simulation */


/***************************************
*          Constants
***************************************/

#define BATTERY_TIMEOUT             (15u)       /*  Сounts in seconds */

#define SIM_BATTERY_MIN             (2u)        /* Minimum simulated battery level measurement */
#define SIM_BATTERY_MAX             (20u)       /* Maximum simulated battery level measurement */
#define SIM_BATTERY_INCREMENT       (1u)        /* Value by which the battery level is incremented */                             

#define MEASURE_BATTERY_MAX         (3000)      /* Use 3V as battery voltage starting */
#define MEASURE_BATTERY_MID         (2800)      /* Use 2.8V as a knee point of discharge curve @ 29% */
#define MEASURE_BATTERY_MID_PERCENT (29)        
#define MEASURE_BATTERY_MIN         (2000)      /* Use 2V as a cut-off of battery life */
#define LOW_BATTERY_LIMIT           (10)        /* Low level limit in percent to switch on LED */
    

#if (BAS_SIMULATE_ENABLE != 0)
#define BAS_SERVICE_SIMULATE        (CYBLE_BATTERY_SERVICE_INDEX)   /* BAS service for simulation */ 
#endif /* (BAS_SIMULATE_ENABLE != 0) */

#if (BAS_MEASURE_ENABLE != 0)
#define BAS_SERVICE_MEASURE         (CYBLE_BATTERY_SERVICE_INDEX)   /* BAS service for measure actual battery level */  
#endif /* (BAS_MEASURE_ENABLE != 0) */

#define ADC_VREF_MASK               (0x000000F0Lu)



/***************************************
*       Function Prototypes
***************************************/
void BasCallBack(uint32 event, void *eventParam);
void BasInit(void);
#if (BAS_MEASURE_ENABLE != 0)
void MeasureBattery(void);
#endif /* BAS_MEASURE_ENABLE != 0 */

#if (BAS_SIMULATE_ENABLE != 0)
void SimulateBattery(void);
#endif /* BAS_SIMULATE_ENABLE != 0 */


/***************************************
* External data references
***************************************/
extern uint16 batterySimulationNotify;
extern uint16 batteryMeasureNotify;


/* [] END OF FILE */
