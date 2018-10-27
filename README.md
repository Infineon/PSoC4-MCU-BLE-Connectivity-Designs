## Table of Contents

* [Introduction](#introduction)
* [Navigating the Repository](#navigating-the-repository)
* [Required Tools](#required-tools)
* [Code Examples List](#code-examples-list)
* [References](#references)

# Introduction

This repository contains examples and demos for [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4) family of devices, world's Most Flexible 32-bit ARM Cortex-M0 One-Chip Solution. Cypress provides a wealth of data at [www.cypress.com](http://www.cypress.com/) to help you select the right PSoC device and effectively integrate it into your design. Visit our [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4) webpage to explore more about PSoC 4 MCU family of device.

Feel free to explore through the code example source files and Let us innovate together!

# Navigating the Repository

The examples in this repository demonstrates PSoC 4 based Bluetooth Low Energy (BLE) connectivity feature. These code examples show how to integrate the complex BLE protocol into your design effortlessly.
If you are new to developing projects with PSoC 4 MCU, we recommend you to refer the [PSoC 4 Getting Started](https://github.com/cypresssemiconductorco/PSoC4-MCU-Getting-Started) GitHub page which can help you familiarize with device features and guides you to create a simple PSoC 4 design with PSoC Creator IDE. For other block specific design please visit the following GitHub Pages:

#### 1. [Analog Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Analog-Designs)
#### 2. [Capacitive Touch Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Capacitive-Touch-Designs)
#### 3. [Digital Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Digital-Designs)
#### 4. [USB Connectivity Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-USB-Connectivity-Designs)
#### 5. [Device Related Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-Device-Related-Designs)
#### 6. [PSoC 4 Pioneer Kit](https://github.com/cypresssemiconductorco/PSoC4-MCU-Pioneer-Kits)
#### 7. [System Designs](https://github.com/cypresssemiconductorco/PSoC4-MCU-System-Designs)

You can use these block level examples to guide you through the development of a system-level design. All the code examples in this repository comes with well documented design guidelines to help you understand the design and how to develop it. The code examples and their associated documentation are in the Code Example folder in the repository.

# Required Tools

## Software
### Integrated Development Environment (IDE)
To use the code examples in this repository, please download and install
[PSoC Creator](http://www.cypress.com/products/psoc-creator)

## Hardware
### PSoC 4 MCU Development Kits
* [CY8CKIT-042-BLE-A Bluetooth® Low Energy 4.2 Compliant Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-042-ble-bluetooth-low-energy-42-compliant-pioneer-kit).

* [CY8CKIT-041-41XX PSoC 4100S CapSense Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-041-41xx-psoc-4100s-capsense-pioneer-kit). 

* [CY8CKIT-046 PSoC 4 L-Series Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-046-psoc-4-l-series-pioneer-kit)

* [CY8CKIT-044 PSoC 4 M-Series Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-044-psoc-4-m-series-pioneer-kit)

* [CY8CKIT-049-4xxx PSoC 4 Prototyping Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits)

* [CY8CKIT-042 PSoC 4 Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/cy8ckit-042-psoc-4-pioneer-kit) 

**Note** Please refer to the code example documnetation for selecting the appropriate kit for testing the project

## Code Example List
#### 1. BLE_Battery_Level
This project demonstrates measurements of the battery voltage using PSoC 4 BLE/PRoC BLE's internal ADC and notifies the BLE central device of any change in the battery voltage.
#### 2. BLE 4.2 Data Length Security Privacy
This example project demonstrates the new BLE 4.2 features of the PSoC Creator BLE component.
#### 3. BLE Alert Notification Profile
This example project demonstrates the Alert Notification Client operation of the BLE PSoC Creator Component. The Alert Notification Client utilizes the BLE Alert Notification Profile with one instance of Alert Notification Service to receive information about “Email”, “Missed call” and “SMS/MMS” alerts from Alert Notification Server. Device remains in Sleep mode between BLE connection intervals.
#### 4. BLE Apple Notification Client
This example project demonstrates the BLE Apple Notification Client application workflow. The application uses the BLE Apple Notification Center Service in GATT Client mode to communicate with a BLE Apple Notification Center Server (iPhone, iPod, etc.).
#### 5. BLE Blood Pressure Sensor
This example project demonstrates the BLE Blood Pressure Sensor application workflow. The Blood Pressure Sensor application utilizes the BLE Blood Pressure profile to report blood pressure measurement records to the Client. Also the Blood Pressure Sensor application utilizes the Battery Service to notify the battery level and the Device Information Service to assert the Device Name, etc.
#### 6. BLE Continuous Glucose Monitoring Sensor
This example project demonstrates the BLE Continuous Glucose Monitoring Sensor application workflow. The application uses the BLE Continuous Glucose Monitoring Profile to report CGM Measurement records to the Client by the Continuous Glucose Monitoring Service and to manage bonding by the Bond Management Service. Also, the application uses the Device Information Service to assert the Device Name, etc. The PSoC 4 BLE and PRoC BLE devices are supported.
#### 7. BLE Cycling Sensor
This example demonstrates the Cycling Speed and Cadence Service (CSCS) and Cycling Power Service (CPS). Cycling Speed and Cadence simulates a cycling activity and reports the simulated cycling speed and cadence data to a BLE central device using CSCS. Cycling Power simulates cycling power data and reports the simulated data to a BLE central device using CPS.
#### 8. BLE Device Information Service
This is a simple BLE example project that demonstrates how to configure and use BLE component APIs and an application layer callback. The Device Information Service is used as an example to demonstrate how to configure the BLE service characteristics in the BLE component.
#### 9. BLE Environment Sensing Profile
This example project demonstrates the Environmental Sensing Profile operation of the BLE PSoC Creator Component. The Environmental Sensor utilizes the Environmental Sensing Profile with one instance of Environmental Sensing and Device Information Services to simulate wind speed measuring. The Environmental Sensor operates with other devices that implement the Environmental Collector Profile. The device switches to the DeepSleep mode between BLE connection intervals. The BLE component supports the PSoC 4 BLE and PRoC BLE family devices.
#### 10. BLE Find Me Profile
This example project demonstrates the Find Me Profile operation of the BLE PSoC Creator Component. The Find Me Target utilizes the Find Me Profile with one instance of Immediate Alert Service to display the alerts if the Client has configured the device for alerting. Find Me Target operates with other devices which implement the Find Me Locator Profile. The device switches to the DeepSleep mode between BLE connection intervals.
#### 11. BLE GLucose Meter
This example project demonstrates the BLE Glucose Meter application workflow. The Glucose Meter application uses the BLE Glucose Profile to report glucose measurement records to the client. Also the Glucose Meter application uses the Battery Service to notify the battery level and the Device Information Service to assert the Device Name, etc.
#### 12. BLE Heart Rate Collector
This example project demonstrates the BLE Heart Rate Collector workflow. The project receives Heart Rate data from any BLE enabled Heart Rate Sensor and indicates that data on any terminal software via UART.
#### 13. BLE Heart Rate Sensor
This example project demonstrates the BLE Heart Rate Sensor workflow. The project simulates Heart Rate data and performs communication with BLE enabled central/client device.
#### 14. BLE HID Keyboard
This project demonstrates keyboard pressing in the boot and protocol mode. The example also demonstrates handling a suspend event from the central device and entering the low power mode when suspended.
#### 15. BLE HID Mouse
This project demonstrates the mouse movement and button click HID reports in the boot and protocol mode. The example also demonstrates handling the suspend event from the central device and enters the low power mode when suspended.
#### 16. BLE IPSP Router
This example project demonstrates the Internet Protocol Support Profile operation of the Bluetooth Low Energy (BLE) PSoC Creator Component. The example project requires CY8CKIT-042-BLE Pioneer Kit. This example demonstrates how to setup an IPv6 communication infrastructure between two devices over a BLE transport using L2CAP channel. Creation and transmission of IPv6 packets over BLE is not part of this example. The example consists of two projects: IPSP Router (GAP Central) and IPSP Node (GAP Peripheral). Router sends generated packets with different content to Node in the loop and validates them with the afterwards received data packet. Node simply wraps received data coming from Router, back to the Router.
#### 17. BLE IPSP Node
This example project demonstrates the Internet Protocol Support Profile operation of the Bluetooth Low Energy (BLE) PSoC Creator Component. The example project requires CY8CKIT-042-BLE Pioneer Kit. This example demonstrates how to setup an IPv6 communication infrastructure between two devices over a BLE transport using L2CAP channel. Creation and transmission of IPv6 packets over BLE is not part of this example. The example consists of two projects: IPSP Router (GAP Central) and IPSP Node (GAP Peripheral). Router sends generated packets with different content to Node in the loop and validates them with the afterwards received data packet. Node simply wraps received data coming from Router, back to the Router.
#### 18. BLE Naviagtion
This example project demonstrates the Location and Navigation Pod application workflow. The application uses a BLE Location and Navigation Profile to report location and navigation information to the client. Also the Location and Navigation Pod application uses the Battery Service to notify the battery level and the Device Information Service to assert the Device Name, etc.
#### 19. BLE OTA External Memory Bootloadable
This example project demonstrates an OTA firmware update using the BLE Bootloader Service that is hidden by default and can be activated by pressing button SW2 while the device is running. By default, this is a regular bootloadable project that contains the BLE component with the Device Information Service. Once the bootloader mode is enabled, this example project is ready for receiving a new image of the bootloadable project and storing it to the external memory.
#### 20. BLE OTA External Memory Bootloadaber
This example project demonstrates an OTA firmware update using the BLE Bootloader Service that is hidden by default and can be activated by pressing button SW2 while the device is running. By default, this is a regular bootloadable project that contains the BLE component with the Device Information Service. Once the bootloader mode is enabled, this example project is ready for receiving a new image of the bootloadable project and storing it to the external memory.
#### 21. BLE Fixed Stack Bootloadable
This example shows how to use the custom linker scripts to share a block of memory between the bootloader and bootloadable projects. It demonstrates how the bootloader can place the API functions so that the bootloadable can also call them. This allows creating an OTA bootloader.
#### 22. BLE Fixed Stack Bootloader
This example shows how to use the custom linker scripts to share a block of memory between the bootloader and bootloadable projects. It demonstrates how the bootloader can place the API functions so that the bootloadable can also call them. This allows creating an OTA bootloader.
#### 23. BLE OTA UpgradableStack HID Keyboard
This example shows how to implement an upgradable Application project (HID keyboard) and upgradable Stack project with the BLE Stack. In addition, the Application project uses the BLE Stack from the Stack project.
#### 24. BLE Upgradable Stack Example Launcher
This example shows how to implement an upgradable Application project (HID keyboard) and upgradable Stack project with the BLE Stack. In addition, the Application project uses the BLE Stack from the Stack project.
#### 24. BLE Upgradable Stack Example Stack
This example shows how to implement an upgradable Application project (HID keyboard) and upgradable Stack project with the BLE Stack. In addition, the Application project uses the BLE Stack from the Stack project.
#### 26. BLE Phone Alert
This example project demonstrates the BLE Phone Alert Handler application workflow. The Phone Alert Handler application uses the BLE Phone Alert Status Profile to monitor and control Alert State and Ringer Setting of the Server (phone).
#### 27. BLE Proximity
This example project demonstrates the Proximity operation of the BLE PSoC Creator Component. The Proximity Reporter utilizes the BLE Proximity Profile with one instance of Link Loss Service and one instance of Tx Power Service to display alerts on the device if connection with Client has been lost. The Proximity Reporter operates with other devices which implement the Proximity Monitor Profile role. The device uses Limited Discovery mode during which it is visible for BLE Clients. The device remains in DeepSleep mode between BLE connection intervals.
#### 28. BLE Running Speed Cadence
This example project demonstrates the Running Speed and Cadence Sensor operation of the BLE PSoC Creator Component. The device simulates running/walking data measurements and sends it over the BLE Running Speed and Cadence Service.
#### 29. BLE Temperature Measurement
This example demonstrates the Health Thermometer Profile operation of the BLE PSoC Creator component. The device simulates thermometer readings and sends it over the BLE Health Thermometer Service. It also measures a battery level value and sends it over the BLE Battery Service.
#### 30. BLE Time Sync
This example project demonstrates the Time profile operation of the BLE PSoC Creator Component. The Time Sync example utilizes the BLE Time Profile (configured for GAP peripheral role as Time Client) with one instance of Current Time Service to demonstrate capability of time synchronization from the external Time Server. The project also contains one instance of Reference Time Update Service and one instance of Next DST Change Service, however they are not used in the example.
The Time Client operates with other devices, which implement the Time Server Profile role. The device uses Limited Discovery mode during which it is visible for BLE Servers. The device remains in the sleep mode between BLE connection intervals.
#### 31. BLE Weight Scale
This example project demonstrates the Weight Scale Profile operation of the BLE PSoC Creator Component. The Weight Scale Sensor utilizes one instance of Weight Scale, Body Composition, User Data and Device Information Services to simulate weight measurements for up to four registered users. The Weight Scale Sensor operates with other devices that implement the Weight Scale Collector Profile. The device switches to the DeepSleep mode between BLE connection intervals. The BLE component supports the PSoC 4 BLE and PRoC BLE family devices.
#### 32. BLE Wireless Power Transmitter
This example project demonstrates the Wireless Power Transfer Profile operation of the BLE PSoC Creator Component. The example project requires CY8CKIT-042-BLE Pioneer Kit. The example consists of two projects: Wireless Power Transmitter (GATT Client) and Wireless Power Receiver (GATT Server).
#### 33. BLE Wireless Power Receiver
This example project demonstrates the Wireless Power Transfer Profile operation of the BLE PSoC Creator Component. The example project requires CY8CKIT-042-BLE Pioneer Kit. The example consists of two projects: Wireless Power Transmitter (GATT Client) and Wireless Power Receiver (GATT Server).

## References
#### 1. PSoC 4 MCU
PSoC 4 is the world's Most Flexible 32-bit ARM Cortex-M0 One-Chip Solution. PSoC 4 has tackled some of the complex portions of embedded system design making it easier for you to get your product to market. Functions such as analog sensor integration, capacitive touch, and wireless connectivity have been integrated and optimized in PSoC 4 to “just work” so you don’t have to. To learn more on the device. Learn more: [PSoC 4 MCU](http://www.cypress.com/products/32-bit-arm-cortex-m0-psoc-4)

####  2. PSoC 4 MCU Learning resource list
##### 2.1 PSoC 4 MCU Datasheets
Device datasheets list the features and electrical specifications of PSoC 4 families of devices: [PSoC 4 MCU Datasheets](http://www.cypress.com/search/all?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=field_related_products%3A1297&f%5b2%5d=resource_meta_type%3A575)
##### 2.2 PSoC 4 MCU Application Notes
Application notes are available on the Cypress website to assist you with designing your PSoC application: [A list of PSoC 4 MCU ANs](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Atechnical_documents%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dresource_meta_type%253A574)
##### 2.3 PSoC 4 MCU Component Datasheets
PSoC Creator utilizes "components" as interfaces to functional Hardware (HW). Each component in PSoC Creator has an associated datasheet that describes the functionality, APIs, and electrical specifications for the HW. You can access component datasheets in PSoC Creator by right-clicking a component on the schematic page or by going through the component library listing. You can also access component datasheets from the Cypress website: [PSoC 4 Component Datasheets](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Asoftware_tools%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dsoftware_tools_meta_type%253A532)
##### 2.4 PSoC 4 MCU Technical Reference Manuals (TRM)
The TRM provides detailed descriptions of the internal architecture of PSoC 4 devices:[PSoC 4 MCU TRMs](https://community.cypress.com/external-link.jspa?url=http%3A%2F%2Fwww.cypress.com%2Fsearch%2Fall%3Ff%255b0%255d%3Dmeta_type%253Atechnical_documents%26f%255b1%255d%3Dfield_related_products%253A1297%26f%255b2%255d%3Dresource_meta_type%253A583)

## FAQ

### Technical Support
Need support for your design and development questions? Check out the [Cypress Developer Community 3.0](https://community.cypress.com/welcome).  

Interact with technical experts in the embedded design community and receive answers verified by Cypress' very best applications engineers. You'll also have access to robust technical documentation, active conversation threads, and rich multimedia content.

You can also use the following support resources if you need quick assistance:

Self-help: [http://www.cypress.com/support](http://www.cypress.com/support)

Local Sales office locations: [http://www.cypress.com/about-us/sales-offices](http://www.cypress.com/about-us/sales-offices)
