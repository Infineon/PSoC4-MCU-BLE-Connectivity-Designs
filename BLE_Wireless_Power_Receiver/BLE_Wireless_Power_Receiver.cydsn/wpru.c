/*******************************************************************************
* File Name: wpru.c
*
* Version: 1.0
*
* Description:
*  This file contains WPT callback handler function and code for Power 
*  Receiver Unit.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
* 
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "common.h"
#include "wpru.h"


/*******************************************************************************
* Global variables
*******************************************************************************/
uint32 chargingTimer;
uint16 wPowerSimulation;
CYBLE_PRU_DYNAMIC_PAR_T pruDynamicParameter;
CYBLE_PRU_CONTROL_T pruControl;

const uint16 maxSourceImpedance[] = {50u, 60u, 70u, 80u, 90u, 100u, 110, 120u, 130u, 140u, 150u,
                                     175u, 200u, 225u, 250u, 275u, 300u, 350u, 375u};

uint32 temperatureTimer = 1u;
uint16 initialMeasurementInterval = 10u;


/*******************************************************************************
* Function Name: Wpts_GetPower
********************************************************************************
*
* Summary:
*   This function converts value in decimal to power in milliwatts.
*
* Parameters:
*  Value in decimal.
*
* Return:
*  Power in milliWatts.
*
*******************************************************************************/
uint32 Wpts_GetPower(uint8 value)
{
    uint8 range = 0u;
    uint32 step = PTU_STATIC_PAR_POWER_STEP;
    uint32 power = 0u;
    while(range < value)
    {
        if((value - range) > PTU_STATIC_PAR_POWER_RANGE)
        {
            power += step * PTU_STATIC_PAR_POWER_RANGE; 
        }
        else
        {
            power += step * (value - range); 
        }
        range += PTU_STATIC_PAR_POWER_RANGE;
        if(range < PTU_STATIC_PAR_POWER_HI_RANGE)
        {
            step += PTU_STATIC_PAR_POWER_STEP;
        }
        else
        {
            step = PTU_STATIC_PAR_POWER_HI_STEP;
        }
    }
    return(power);
}


/*******************************************************************************
* Function Name: Wpts_CallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
*******************************************************************************/
void Wpts_CallBack(uint32 event, void* eventParam)
{
    uint8 locCharIndex;
    
    locCharIndex = ((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->charIndex;
    
    switch(event)
    {
        /* WPTS Server - Notifications for Wireless Power Transfer Service Characteristic
            were enabled. The parameter of this event is a structure of
            CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_WPTS_NOTIFICATION_ENABLED: char: %x\r\n", locCharIndex);
            if(locCharIndex == CYBLE_WPTS_PRU_ALERT)
            {
                wPowerSimulation |= PRU_ALERT_NOTIFICATION_ENABLE;
            }
            break;
            
        /* WPTS Server - Notifications for Wireless Power Transfer Service Characteristic
            were disabled. The parameter of this event is a structure 
            of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_WPTS_NOTIFICATION_DISABLED: char: %x\r\n", locCharIndex);
            if(locCharIndex == CYBLE_WPTS_PRU_ALERT)
            {
                wPowerSimulation &= ~PRU_ALERT_NOTIFICATION_ENABLE;
            }
            break;
            
        /* WPTS Server - Indication for Wireless Power Transfer Service Characteristic
            was enabled. The parameter of this event is a structure 
            of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSS_INDICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_WPTS_INDICATION_ENABLED: char: %x\r\n", locCharIndex);
            if(locCharIndex == CYBLE_WPTS_PRU_ALERT)
            {
                wPowerSimulation |= PRU_ALERT_INDICATION_ENABLE;
            }
            break;
            
        /* WPTS Server - Indication for Wireless Power Transfer Service Characteristic
            was disabled. The parameter of this event is a structure 
            of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSS_INDICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_WPTS_INDICATION_DISABLED: char: %x\r\n", locCharIndex);
            if(locCharIndex == CYBLE_WPTS_PRU_ALERT)
            {
                wPowerSimulation &= ~PRU_ALERT_INDICATION_ENABLE;
            }
            break;
            
        /* WPTS Server - Wireless Power Transfer Service Characteristic
            Indication was confirmed. The parameter of this event
            is a structure of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSS_INDICATION_CONFIRMED:
            DBG_PRINTF("CYBLE_EVT_WPTS_INDICATION_CONFIRMED\r\n");
            break;
            
        /* WPTS Server - Write Request for Wireless Power Transfer Service Characteristic 
            was received. The parameter of this event is a structure
            of CYBLE_WPTS_CHAR_VALUE_T type.
        */    
        case CYBLE_EVT_WPTSS_WRITE_CHAR:
            if(locCharIndex ==CYBLE_WPTS_PRU_CONTROL)
            {
                pruControl = *(CYBLE_PRU_CONTROL_T *)(((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->value->val);
                DBG_PRINTF("PRU_CONTROL: enables: %x, Output - ", pruControl.enables);
                if((pruControl.enables & PRU_CONTROL_ENABLES_ENABLE_PRU_OUTPUT) != 0u)
                {
                    DBG_PRINTF("ENABLE ,");
                }
                else
                {
                    DBG_PRINTF("DISABLE,");
                }
                DBG_PRINTF("Charge indicator -");
                if((pruControl.enables & PRU_CONTROL_ENABLES_ENABLE_CHARGE_INDICATOR) != 0u)
                {
                    DBG_PRINTF("ENABLE ,");
                    chargingTimer = 0u;
                    pruDynamicParameter.alert &= (uint8)~(PRU_ALERT_CHARGE_COMPLETE | PRU_ALERT_OVER_MASK);
                    (void)CyBle_WptssSetCharacteristicValue(CYBLE_WPTS_PRU_DYNAMIC_PAR, sizeof(pruDynamicParameter), 
                        (uint8 *)&pruDynamicParameter);
                    Charging_LED_Write(LED_ON);
                }
                else
                {
                    DBG_PRINTF("DISABLE,");
                    Charging_LED_Write(LED_OFF);
                }
                DBG_PRINTF("Power:");
                switch(pruControl.enables & PRU_CONTROL_ENABLES_ADJUST_POWER_MASK)
                {
                    case PRU_CONTROL_ENABLES_ADJUST_POWER_MAX: 
                        DBG_PRINTF("Max");
                        break;
                    case PRU_CONTROL_ENABLES_ADJUST_POWER_66:
                        DBG_PRINTF("66 percent");
                        break;
                    case PRU_CONTROL_ENABLES_ADJUST_POWER_33:
                        DBG_PRINTF("33 percent");
                        break;
                    case PRU_CONTROL_ENABLES_ADJUST_POWER_2_5:
                        DBG_PRINTF("2.5W");
                        break;
                }                    
                DBG_PRINTF(" permission: %x, ", pruControl.permission);
                if((pruControl.permission & PRU_CONTROL_PERMISSION_DENIED_FLAG) != 0u)
                {
                    DBG_PRINTF("Denied ,");
                }
                else if((pruControl.permission & PRU_CONTROL_PERMISSION_PERMITTED_WITH_WARNING) != 0u)
                {
                    DBG_PRINTF("Permitted with warning,");
                }
                else
                {
                    DBG_PRINTF("Permitted,");
                }
                DBG_PRINTF("timeSet %d ms\r\n", pruControl.timeSet * PRU_CONTROL_TIME_SET_STEP_MS);
            }
            else if(locCharIndex ==CYBLE_WPTS_PTU_STATIC_PAR)
            {
                CYBLE_PTU_STATIC_PAR_T ptuStaticPar;
                ptuStaticPar = *(CYBLE_PTU_STATIC_PAR_T *)(((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->value->val);
                DBG_PRINTF("PTU_STATIC_PARAMETER: flags: %x, power: %.1f watts, ", 
                    ptuStaticPar.flags, (float)Wpts_GetPower(ptuStaticPar.ptuPower)/1000);
                if(((ptuStaticPar.flags & PTU_STATIC_PAR_FLAGS_MAX_IMPEDANCE_EN) != 0u) &&
                   (ptuStaticPar.ptuMaxSourceImpedance <= PTU_STATIC_PAR_MAX_SOURCE_IMPEDANCE_MAX))
                {
                    DBG_PRINTF("maxSourceImpedance: %d ohms, ", maxSourceImpedance[ptuStaticPar.ptuMaxSourceImpedance]);
                }
                if((ptuStaticPar.flags & PTU_STATIC_PAR_FLAGS_MAX_RESISTANCE_EN) != 0u)
                {
                    DBG_PRINTF("maxLoadResistance: %d ohms, ", Wpts_GetLoadResistance(ptuStaticPar.ptuMaxLoadResistance));
                }
                DBG_PRINTF("supported devices number: %d , ", ptuStaticPar.ptuDevNumber + PTU_STATIC_PAR_NUMBER_OF_DEVICES_OFFSET);
                DBG_PRINTF("class: Class%d , ", ptuStaticPar.ptuClass + PTU_STATIC_PAR_CLASS_OFFSET);
                DBG_PRINTF("hardware rev: %d, firmware rev: %d, protocol rev: %d\r\n", 
                    ptuStaticPar.hardwareRev, ptuStaticPar.firmwareRev, ptuStaticPar.protocolRev);
            }
            else
            {
                DBG_PRINTF("CYBLE_EVT_WPTS_CHAR_WRITE: char: %x, value: ", locCharIndex);
                ShowValue(((CYBLE_WPTS_CHAR_VALUE_T *)eventParam)->value);
            }
            break;
            
        /* WPTS Client - Wireless Power Transfer Service Characteristic
            Indication was received. The parameter of this event
            is a structure of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSC_INDICATION:
            break;
            
        /* WPTS Client - Read Response for Read Request of Wireless Power Transfer 
            Service Characteristic value. The parameter of this event
            is a structure of CYBLE_WPTS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSC_READ_CHAR_RESPONSE:
            break;
            
        /* WPTS Client - Read Response for Read Request of Wireless Power Transfer
            Service Characteristic Descriptor Read request. The 
            parameter of this event is a structure of
            CYBLE_WPTS_DESCR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSC_READ_DESCR_RESPONSE:
            break;
            
        /* WPTS Client - Write Response for Write Request of Wireless Power Transfer
            Service Characteristic Configuration Descriptor value.
            The parameter of this event is a structure of 
            CYBLE_WPTS_DESCR_VALUE_T type.
        */
        case CYBLE_EVT_WPTSC_WRITE_DESCR_RESPONSE:
            break;
		default:
            DBG_PRINTF("WPTS OTHER event: %lx \r\n", event);
			break;
    }
}


/*******************************************************************************
* Function Name: SetAlert()
********************************************************************************
*
* Summary:
*   This function send Notification and/or indication of Alert characteristic 
*   if it is enabled by client and stores Alert value in PRU Dynamic Parameter
*   characteristic. 
*
* Parameters:
*   uint8 alert value. Combination of the following defines:
*       PRU_ALERT_OVER_VOLTAGE
*       PRU_ALERT_OVER_CURRENT
*       PRU_ALERT_OVER_TEMP
*       PRU_ALERT_SELF_PROTECTION
*       PRU_ALERT_CHARGE_COMPLETE
*       PRU_ALERT_WIRED_CHARGER_DETECT
*
* Return:
*   None
*
*******************************************************************************/
void SetAlert(uint8 alert)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    pruDynamicParameter.alert |= alert;
        
    if(wPowerSimulation & PRU_ALERT_NOTIFICATION_ENABLE)
    {
        apiResult = CyBle_WptssSendNotification(cyBle_connHandle, CYBLE_WPTS_PRU_ALERT, 
            sizeof(pruDynamicParameter.alert), &pruDynamicParameter.alert);
        DBG_PRINTF("WptssSendNotification Alert: %x  ", alert);
        if((apiResult != CYBLE_ERROR_OK))
        {
            DBG_PRINTF("CpssSendNotification API Error: %x \r\n", apiResult);
        }
    }
    if(wPowerSimulation & PRU_ALERT_INDICATION_ENABLE)
    {
        apiResult = CyBle_WptssSendIndication(cyBle_connHandle, CYBLE_WPTS_PRU_ALERT, 
            sizeof(pruDynamicParameter.alert), &pruDynamicParameter.alert);
        DBG_PRINTF("WptssSendNotification Alert: %x ", alert);
        if((apiResult != CYBLE_ERROR_OK))
        {
            DBG_PRINTF("CpssSendNotification API Error: %x \r\n", apiResult);
        }
    }
    /* Update alert value in PRU Dynamic Parameter characteristic */
    apiResult = CyBle_WptssSetCharacteristicValue(CYBLE_WPTS_PRU_DYNAMIC_PAR, sizeof(pruDynamicParameter), 
        (uint8 *)&pruDynamicParameter);
    if((apiResult != CYBLE_ERROR_OK))
    {
        DBG_PRINTF("CyBle_WptssSetCharacteristicValue API Error: %x \r\n", apiResult);
    }
}


/*******************************************************************************
* Function Name: SimulateWirelessTransfer()
********************************************************************************
*
* Summary:
*   A custom function for service simulation. It generate Alert notification 
*   with ahrge complete status after timeout period. 
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void SimulateWirelessTransfer(void)
{
    if(++chargingTimer > PRU_CHARGING_TIME)
    {
        chargingTimer = 0;
        SetAlert(PRU_ALERT_CHARGE_COMPLETE);
        UpdateLedState();
    }
}


/*******************************************************************************
* Function Name: SimulateWirelessTransfer()
********************************************************************************
*
* Summary:
*   This function measures wireless power transfer system parameters (Vrect, 
*   Irect, Vout, Iout, Temperature) and stores them to the PRU Dynamic Parameter 
*   characteristic.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void MeasureWirelessParam(void)
{

	int32 ADCCountsCorrected;
    int32 temperatureCelsius;
    CYBLE_API_RESULT_T apiResult;
    int16 vOut;
    int16 iOut;
    int16 vRect;
    int16 iRect;
    uint8 alert = 0;
    
    /* Measure wireless power transfer system parameters */
  	ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    
    /* Read voltage results and convert to millivolts */
    vOut = ADC_CountsTo_mVolts(ADC_VOUT_CHANNEL, ADC_GetResult16(ADC_VOUT_CHANNEL));
    vRect = ADC_CountsTo_mVolts(ADC_VRECT_CHANNEL, ADC_GetResult16(ADC_VRECT_CHANNEL));
    
    /* Read current results and convert to milliampers */
    iOut = ADC_CountsTo_mVolts(ADC_IOUT_CHANNEL, ADC_GetResult16(ADC_IOUT_CHANNEL)) / ADC_IOUT_SHUNT;
    iRect = ADC_CountsTo_mVolts(ADC_IRECT_CHANNEL, ADC_GetResult16(ADC_IRECT_CHANNEL)) / ADC_IRECR_SHUNT;

    /* Cut negative results */
    if(vOut < 0)
    {
        vOut = 0;
    }
    if(iOut < 0)
    {
        iOut = 0;
    }
    if(vRect < 0)
    {
        vRect = 0;
    }
    if(iRect < 0)
    {
        iRect = 0;
    }
    
    /* Copy results to characteristic */
    pruDynamicParameter.vOut = (uint16)vOut;
    pruDynamicParameter.iOut = (uint16)iOut;
    pruDynamicParameter.vRect = (uint16)vRect;
    pruDynamicParameter.iRect = (uint16)iRect;

    DBG_PRINTF("Measure Vrect: %d mV, Irect: %d mA, Vout: %d mV, Iout: %d mA", vRect, iRect, vOut, iOut);

    /* Measure Die temperature rarely */
    if((--temperatureTimer == 0u)) 
    {
        temperatureTimer = initialMeasurementInterval;
        
        /* Enables injection channel and performs a measurement. */
        ADC_EnableInjection();
    	ADC_StartConvert();
    	ADC_IsEndConversion(ADC_WAIT_FOR_RESULT_INJ);
        
        /* Read conversion result and adjust data from ADC with respect to Vref value. */
    	ADCCountsCorrected = ADC_DEFAULT_VREF_MV_VALUE * ADC_GetResult16(ADC_TEMPERATURE_CHANNEL) / ADC_DEF_TEMP_REF;
        
        /* Convert the ADC output to degrees Celsius. */
        temperatureCelsius = DieTemp_CountsTo_Celsius(ADCCountsCorrected);

        if(temperatureCelsius < -PRU_DYNAMIC_PAR_TEMPERATURE_OFFSET)
        {
            temperatureCelsius = -PRU_DYNAMIC_PAR_TEMPERATURE_OFFSET;
        }
        /* Copy temperature to characteristic */
         pruDynamicParameter.temperature = (uint8)((temperatureCelsius + PRU_DYNAMIC_PAR_TEMPERATURE_OFFSET) & 0xFF);

         DBG_PRINTF(", Temperature: %d C", (int16)temperatureCelsius);
        
    }
    
    if(pruDynamicParameter.vRect > PRU_VRECT_MAX)
    {
        alert |= PRU_ALERT_OVER_VOLTAGE;
    }
    if(pruDynamicParameter.iRect > PRU_IRECT_MAX)
    {
        alert |= PRU_ALERT_OVER_CURRENT;
    }
    if(pruDynamicParameter.temperature > (PRU_OVER_TEMP_LEVEL + PRU_DYNAMIC_PAR_TEMPERATURE_OFFSET))
    {
        alert |= PRU_ALERT_OVER_TEMP;
    }
    
    if(alert != 0u)
    {
        SetAlert(PRU_ALERT_OVER_VOLTAGE);
        UpdateLedState();
    }
    
    DBG_PRINTF("\r\n");
    
    /* Write PRU Dynamic Parameter characteristic value */
    apiResult = CyBle_WptssSetCharacteristicValue(CYBLE_WPTS_PRU_DYNAMIC_PAR, sizeof(pruDynamicParameter), 
        (uint8 *)&pruDynamicParameter);
    if((apiResult != CYBLE_ERROR_OK))
    {
        DBG_PRINTF("CyBle_WptssSetCharacteristicValue API Error: %x \r\n", apiResult);
    }
    
}


/*******************************************************************************
* Function Name: BLE_WptsInit()
********************************************************************************
*
* Summary:
*   Initializes the WPT service.
*
*******************************************************************************/
void WptsInit(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 cccdValue;

    chargingTimer = 0u;
    wPowerSimulation = PRU_ALERT_SIMULATION_DISABLE;

    /* Register service-specific callback function */
    CyBle_WptsRegisterAttrCallback(Wpts_CallBack);

    /* Read CCCD configurations from flash */
    apiResult = CyBle_WptssGetCharacteristicDescriptor(CYBLE_WPTS_PRU_ALERT, CYBLE_WPTS_CCCD, 
        CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if(apiResult == CYBLE_ERROR_OK)
    {
        wPowerSimulation = cccdValue;
    }
    else
    {
        DBG_PRINTF("CyBle_WptssGetCharacteristicDescriptor API Error: %x \r\n", apiResult);
    }
    /* Read PRU Dynamic Parameter characteristic default value */
    apiResult = CyBle_WptssGetCharacteristicValue(CYBLE_WPTS_PRU_DYNAMIC_PAR, sizeof(pruDynamicParameter), 
        (uint8 *)&pruDynamicParameter);
    if((apiResult != CYBLE_ERROR_OK))
    {
        DBG_PRINTF("CyBle_WptssGetCharacteristicValue API Error: %x \r\n", apiResult);
    }
    
}

/* [] END OF FILE */

